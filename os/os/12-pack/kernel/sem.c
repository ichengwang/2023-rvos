
/*---------------------------- Include ---------------------------------------*/
#include "os.h"

sem_t           SemTbl[MAX_SEM]= {{0}}; 
uint8_t         SemBitmap[MAX_SEM_BITMAP] = 0; 


static void semTaskToRdy(sem_t *psem)
{
    task_t *ptcb = psem->semTCBList.next;

    if (ptcb == psem->semTCBList) //empty list
        return
    else {
        spin_lock();
        ptcb->state = TASK_READY;
        task_resume(ptcb);
        spin_unlock();
    }
}

static void semAllTaskToRdy(sem_t *psem)
{
    task_t *ptcb = psem->semTCBList.next;

    while(ptcb != psem->semTCBList) 
    {
        spin_lock();
        ptcb->state = TASK_READY;
        task_resume(ptcb);
        ptcb = psem->semTCBList.next;
        spin_unlock();
    }
}

static void semTaskToWait(sem_t *psem, task_t *curTCB)
{
    switch(psem->queueType) 
    {
        case PRIO:
            task_t *ptcb = psem->semTCBList.next;
            while(ptcb!=psem->semTCBList) {
                if (ptcb->current_priority <= curTCB->current_priority) //find next
                    ptcb = ptcb->next;
                else {
                    list_insert_before(ptcb, curTCB);
                    break;
                }
            }
            break;
        default://FIFO
            list_insert_before(psem->semTCBList);
            break;
    }
}

int create_sem(uint8_t sortType, uint8_t value)
{
    uint32_t i;
    uint32_t index;
    uint8_t offset;
    
    spin_lock();
    for(i = 0; i < MAX_SEM; i++)
    {
        index = i / 8;
        offset = i % 8;
        if((SemBitmap[index] & (1 << offset)) == 0) /* Is free ID?                */
        {
            SemBitmap[index] |= (1<<offset);        
            spin_unlock();
            SemTbl[i].value       = value;      
            SemTbl[i].queueType   = sortType;	
            list_init((list_t *) (&SemTbl[i]));
            return i;                           /* Return  ID                  */
        }
    }
    spin_unlock();                      /* Unlock schedule                  */
    return -1;                        /* Error return                     */
}


StatusType delete_sem(uint32_t semID)
{	
    uint32_t index;
    uint8_t offset;
    index = semID / 8;
    offset = semID % 8;
    SemBitmap[index] &=~(1<<offset);  
    sem_t *[sem] = &SemTbl[semID];
    semAllTaskToRdy(psem);      
    return OK;                      /* Return OK                            */
}



/**
 *******************************************************************************
 * @brief      Accept a semaphore without waitting 	  
 *
 * @par Description
 * @details    This function is called accept a semaphore without waitting. 
 *******************************************************************************
 */
StatusType getSem_imm(uint32_t semID)
{

    uint32_t index;
    uint8_t offset;
    index = semID / 8;
    offset = semID % 8;

    if (!(SemBitmap[index] & (1<<offset)))
        return -1;
    sem_t *psem = SemTbl[semID]; 

	spin_lock();
    if (psem->value > 0) {
        psem->value--;
        spin_unlock();
        return E_OK;
    }else {
        spin_unlock();
        return -1;
    }
}

 
/**
 *******************************************************************************
 * @brief       wait for a semaphore	   
 * @param[in]   id       Event ID.	
 * @param[in]   timeout  The longest time for writting semaphore.
 *******************************************************************************
 */
StatusType getSem_wait(uint32_t semID, uint32_t timeout)
{
    uint32_t index;
    uint8_t offset;
    index = semID / 8;
    offset = semID % 8;

    if (!(SemBitmap[index] & (1<<offset)))
        return -1;
    sem_t *psem = SemTbl[semID]; 

	spin_lock();
    if (psem->value > 0) {
        psem->value--;
        spin_unlock();
        return E_OK;
    }else {
        curTCB = TCBRunning;
        if (timeout == 0) //waiting in sem queue
        {
            semTaskToWait(psem, curTCB);
            psem->value--;
            curTCB->error = OK;
            spin_unlock();
            return OK;
        } else {  // queue in timer delay
            semTaskToWait(psem, curTCB);
            SetTmrCnt(curTCB->timer, timeout, timeout);
            start_timer(curTCB->timer);
            spin_unlock();

            if (curTCB->error == TIMEOUT)
            {
                return -1;
            }
            else {
                stop_timer(curTCB->timer);
                spin_lock();
                curTCB->error = OK;
                psem->value--;
                spin_unlock();
                return OK;
            }
        }
        spin_unlock();
        return -1;
    }
}


/**
 *******************************************************************************
 * @brief       Post a semaphore	 
 *******************************************************************************
 */
StatusType post_sem(uint32_t semID)
{
    uint32_t index;
    uint8_t offset;
    index = semID / 8;
    offset = semID % 8;

    if (!(SemBitmap[index] & (1<<offset)))
        return -1;
    sem_t *psem = SemTbl[semID]; 


    /* Make sure semaphore will not overflow */
    if(psem->value == pecb->initialValue) 
    {
        return -1;    /* The counter of Semaphore reach the max number*/
    }
    spin_lock();
    psem->value++;
    semTaskToRdy(psem);     /* Check semaphore event waiting list           */
    spin_unlock();
    return OK;
		
}

