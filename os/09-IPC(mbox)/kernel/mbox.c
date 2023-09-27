#include "os.h"

/*!< Table use to save MAIL_BOX              */
static MboxCB_t      MboxTbl[MAX_MBOX];
static uint32_t     MboxMap[MAX_MBOX/MAP_SIZE];

/**
static functions
*/
static void _TaskToWait_mbox(uint16_t mboxID, taskCB_t *ptcb)
{
    MboxCB_t *pmboxcb = &MboxTbl[mboxID];
    /* suspend thread */
    task_suspend(ptcb);

    switch (pmboxcb->sortType)
    {
    case FIFO:
        list_insert_before(&(pmboxcb->node), (list_t*)ptcb);
        break;

    case PRIO:
        {
            list_t *plist = pmboxcb->node.next;
            while(plist != (list_t *)pmboxcb) {
                taskCB_t *ptcbNow = (taskCB_t *)plist;
                if (ptcbNow->priority > ptcb->priority) 
                    break;
                plist = plist->next;
            }
            list_insert_before(plist, (list_t *)ptcb);
        }
        break;

    default:
        break;
    }
}

static void _WaitTaskToRdy_mbox(uint16_t id)
{
    MboxCB_t *pmboxcb = &MboxTbl[id];
    
    if (list_isempty((list_t*)pmboxcb))
        return;
    
    taskCB_t *ptcb = (taskCB_t *)pmboxcb->node.next;
    if(ptcb == getCurrentTask()){
	    ptcb->state = TASK_RUNNING;
	} else {
        task_resume(ptcb);
    }
}

static void _AllWaitTaskToRdy_mbox(uint16_t id)
{
    MboxCB_t *pmboxcb = &MboxTbl[id];
    taskCB_t *ptcb;
    reg_t lock_status;
    
    while(!list_isempty((list_t*)pmboxcb)) {
        lock_status = spin_lock();
        //get next task
        ptcb = (taskCB_t*) pmboxcb->node.next;
        list_remove((list_t*)ptcb);
        ptcb->returnMsg = E_TIMEOUT;
        //set error for return task???
        //to do
        task_resume(ptcb);
        spin_unlock(lock_status);
    }
}


/***
 interface 
*/
err_t createMbox(uint8_t sortType)
{
    reg_t lock_status;
    lock_status = spin_lock();
    for (int i=0; i<MAX_MBOX;i++) {
        int mapIndex = i / MAP_SIZE;
        int mapOffset = i % MAP_SIZE;
        if ((MboxMap[mapIndex] & (1<< mapOffset)) == 0)
        {
            MboxMap[mapIndex] |= (1<< mapOffset);
            MboxTbl[i].id = i;
            MboxTbl[i].mailPtr = NULL;
            MboxTbl[i].sortType = sortType;
            list_init((list_t*)&MboxTbl[i].node);
            return i;
        }
    }
    spin_unlock(lock_status);
    return E_CREATE_FAIL;
}

 
void delMbox(uint16_t mboxID)
{
    /* wakeup all suspended threads */
    _AllWaitTaskToRdy_mbox(mboxID);
    
    /* free mbox control block */
    int mapIndex = mboxID / MAP_SIZE;
    int mapOffset = mboxID % MAP_SIZE;
    reg_t lock_status = spin_lock();
    MboxMap[mapIndex] &=~(1<<mapOffset);   
    spin_unlock(lock_status);                               
}


err_t postMbox(uint16_t mboxID, void *pmail){
    MboxCB_t *pmboxcb = &MboxTbl[mboxID];

    if (pmboxcb->mailCount == 0) //mailbox empty
    {
        reg_t lock_status=spin_lock();
        pmboxcb->mailPtr = pmail;
        pmboxcb->mailCount = 1;
        //check waiting list
        _WaitTaskToRdy_mbox(mboxID);
        spin_unlock(lock_status);
        return E_OK;
    } else {
        return E_FULL;
    }
}
    
/*
 cal to accept a mail, no wait
*/
void * acceptMail(uint16_t mboxID, err_t *perr) {
    MboxCB_t *pmboxcb = &MboxTbl[mboxID];
    void *pmail;
    reg_t lock_status = spin_lock();
    if (pmboxcb->mailCount == 1) {
        *perr = E_OK;
        pmail = pmboxcb->mailPtr;
        pmboxcb->mailPtr = NULL;
        pmboxcb->mailCount = 0;
        spin_unlock(lock_status);
        return pmail;
    } else {
        spin_unlock(lock_status);
        *perr = E_EMPTY;
        return NULL;
    }
}

/*
called to wait a mail
timeout >0, 0 for ever
*/
void * waitMail(uint16_t mboxID, uint32_t timeout, err_t *perr) {
    MboxCB_t *pmboxcb = &MboxTbl[mboxID];
    void *pmail;
    reg_t lock_status = spin_lock();
    taskCB_t *pcurrentTask;

    if (pmboxcb->mailCount == 1) {
        *perr = E_OK;
        pmail = pmboxcb->mailPtr;
        pmboxcb->mailPtr = NULL;
        pmboxcb->mailCount = 0;
        spin_unlock(lock_status);
        return pmail;
    } else {
        spin_unlock(lock_status);
        pcurrentTask = getCurrentTask();
        pcurrentTask->returnMsg = E_OK;
        if (timeout==0) { //wait forever until mbox deleted or has a mail
            _TaskToWait_mbox(mboxID, pcurrentTask);
            task_yield();
            //wake up here
            if (pcurrentTask->returnMsg == E_OK) {
                *perr = E_OK;
                lock_status = spin_lock();
                pmail = pmboxcb->mailPtr;
                pcurrentTask->pmail = NULL;
                pmboxcb->mailCount = 0;            
                spin_unlock(lock_status);
                return pmail;
            }else{
                //mbox delete
                *perr = E_TIMEOUT;
                return NULL;
            }
        } else { //wait timeout ticks 
            lock_status = spin_lock();
            _TaskToWait_mbox(mboxID, pcurrentTask); //tcb wait in mbox queue
             //timer of task waiting in delayList
            setCurTimerCnt(pcurrentTask->timer->timerID,timeout, timeout);
            startTimer(pcurrentTask->timer->timerID);
            spin_unlock(lock_status);
            //wake up here if timeout or mbox delete or has a mail
            if (pcurrentTask->returnMsg == E_OK) { //wake up from mbox queue
                stopTimer(pcurrentTask->timer->timerID); //remove from delayList
                *perr = E_OK;
                lock_status = spin_lock();
                pmail = pmboxcb->mailPtr;
                pcurrentTask->pmail = NULL;
                pmboxcb->mailCount = 0;            
                spin_unlock(lock_status);
                return pmail;
            }else{
                //mbox delete or timeout
                //remove from mbox list task_resume is called by  taskTimeout
                *perr = E_TIMEOUT;
                return NULL;
            }
        }
    }
}