#include "osQueue.h"

queue_t   QueueTbl[MAX_QUEUE] = {{0}};    /*!< Queue control block table        */
uint8_t   qBitmap[MAX_queue_BITMAP] = 0; 



static void qTaskToRdy(queue_t *pqcb)
{
    task_t *ptcb = pqcb->qTCBList.next;

    if (ptcb == pqcb->qTCBList) //empty list
        return
    else {
        spin_lock();
        ptcb->state = TASK_READY;
        task_resume(ptcb);
        spin_unlock();
    }
}

static void qAllTaskToRdy(queue_t *pqcb)
{
    task_t *ptcb = pqcb->semTCBList.next;

    while(ptcb != pqcb->semTCBList) 
    {
        spin_lock();
        ptcb->state = TASK_READY;
        task_resume(ptcb);
        ptcb = pqcb->qTCBList.next;
        spin_unlock();
    }
}

static void qTaskToWait(queue_t *pqcb, task_t *curTCB)
{
    switch(pqcb->queueType) 
    {
        case PRIO:
            task_t *ptcb = pqcb->qTCBList.next;
            while(ptcb!=pqcb->qTCBList) {
                if (ptcb->current_priority <= curTCB->current_priority) //find next
                    ptcb = ptcb->next;
                else {
                    list_insert_before(ptcb, curTCB);
                    break;
                }
            }
            break;
        default://FIFO
            list_insert_before(pqcb->qTCBList);
            break;
    }
}


int create_sem(void **qStart, uint8_t sortType, unit16_t size, uint8_t sortType)
{
    uint32_t i;
    uint32_t index;
    uint8_t offset;
    
    spin_lock();
    for(i = 0; i < MAX_SEM; i++)
    {
        index = i / 8;
        offset = i % 8;
        if((qBitmap[index] & (1 << offset)) == 0) /* Is free ID?                */
        {
            qBitmap[index] |= (1<<offset);        
            spin_unlock();
            QueueTbl[i].qStart   = qStart;  /* Initialize the queue           */
            QueueTbl[i].queueType = sortType;
            QueueTbl[i].head     = 0;
            QueueTbl[i].tail     = 0;
            QueueTbl[i].qMaxSize = size; 
            QueueTbl[i].qSize    = 0;
            list_init((list_t *) (&QueueTbl[i]));
            return i;                           /* Return  ID                  */
        }
    }
    spin_unlock();                      /* Unlock schedule                  */
    return -1;                        /* Error return                     */
}


StatusType delete_queue(uint32_t qID)
{	
    uint32_t index;
    uint8_t offset;
    index = qID / 8;
    offset = qID % 8;
    queue_t *q = &QueueTbl[qID];

    spin_lock();
    qBitmap[index] &=~(1<<offset);      
    q->qStart   = Co_NULL;
    q->id       = 0;
    q->head     = 0;
    q->tail     = 0;
    q->qMaxSize = 0;
    q->qSize    = 0;
    spin_unlock();
    qAllTaskToRdy(q);      
    return OK;                      /* Return OK                            */
}

 
void* getMail_imm(uint32_t qid, StatusType* perr)
{
  queue_t *pqcb;
  void* pmail;

    pqcb = &QueueTbl[qid];
    spin_lock();
    if(pqcb->qSize != 0)            /* If there are any messages in the queue */
    {
        /* Extract oldest message from the queue */
        pmail = *(pqcb->qStart + pqcb->head);  
        pqcb->head++;                   /* Update the queue head              */ 
        pqcb->qSize--;          /* Update the number of messages in the queue */  
        if(pqcb->head == pqcb->qMaxSize)
        {
            pqcb->head = 0;	
        }
	spin_unlock();
        *perr = E_OK;
        return pmail;                   /* Return message received            */
    }
    else                                /* If there is no message in the queue*/
    {
	spin_unlock();
        *perr = E_QUEUE_EMPTY;                 
        return NULL;                    /* Return Co_NULL                        */
    }	
}

void* getMail_wait(uint32_t qid, uint32_t timeout, StatusType* perr)
{
    queue_t *  pqcb;
    task_t * curTCB;
    void*   pmail;

    pqcb = &QueueTbl[qid];
    spin_lock();    
    if(pqcb->qSize != 0)            /* If there are any messages in the queue */
    {
        /* Extract oldest message from the queue                              */
        pmail = *(pqcb->qStart + pqcb->head);   
        pqcb->head++;                   /* Update the queue head              */ 
        pqcb->qSize--;          /* Update the number of messages in the queue */  
        if(pqcb->head == pqcb->qMaxSize)/* Check queue head                   */
        {
            pqcb->head = 0;	
        }
        spin_unlock();
        *perr = E_OK;
        return pmail;                   /* Return message received            */
    }
    else                                /* If there is no message in the queue*/
    {
    	spin_unlock();
        curTCB = TCBRunning;
        if(timeout == 0)                /* If time-out is not configured      */
        {
            /* Block current task until the event occur                       */
            qTaskToWait(pqcb, curTCB);

            
            /* Have recived message or the queue have been deleted            */
            spin_lock();
            pmail = curTCB->pmail;              
            curTCB->pmail = NULL;
            pqcb->head++;                             /* Clear event sign         */
            pqcb->qSize--;
            if(pqcb->head == pqcb->qMaxSize)
            {
                pqcb->head = 0;	
            }
            spin_unlock();
            *perr = E_OK;
            return pmail;               /* Return message received or Co_NULL    */
        }
        else                            /* If time-out is configured          */
        {
            OsSchedLock(); 
            
            /* Block current task until event or timeout occurs               */           
            qTaskToWait(pqcb, curTCB);
            SetTmrCnt(curTCB->timer, timeout, timeout);
            start_timer(curTCB->timer);
            spin_unlock();
            if(curTCB->pmail == NULL)   /* If time-out occurred               */
            {
                *perr = E_TIMEOUT;
                return NULL;
            }
            else                        /* If event occured                   */
            {
                spin_lock();
                pmail = curTCB->pmail;              
                curTCB->pmail = Co_NULL;
                pqcb->head++;                             /* Clear event sign         */
                pqcb->qSize--;
                if(pqcb->head == pqcb->qMaxSize)
                {
                    pqcb->head = 0;	
                }
                spin_unlock();
                *perr = E_OK;
                return pmail;           /* Return message received or Co_NULL    */
            }				
        }	
    }
}


StatusType postQueueMail(uint32_t qid,void* pmail)
{	
    queue_t * pqcb = &QueueTbl[qid];

    if(pqcb->qSize == pqcb->qMaxSize)   /* If queue is full                   */
    {
        return E_QUEUE_FULL;
    }
    else                                /* If queue is not full               */
    {
        spin_lock();
        *(pqcb->qStart + pqcb->tail) = pmail;   /* Insert message into queue  */
        pqcb->tail++;                           /* Update queue tail          */
        pqcb->qSize++;          /* Update the number of messages in the queue */
        if(pqcb->tail == pqcb->qMaxSize)        /* Check queue tail           */   
        {
            pqcb->tail = 0;	
        }
	qTaskToRdy(psem);     /* Check semaphore event waiting list           */
	spin_unlock();
        return E_OK;
    }
}


