#include "os.h"

/*!< Table use to save SEM              */
static SemCB_t      SEMTbl[MAX_SEM];
static uint32_t     SemMap[MAX_SEM/MAP_SIZE];

/**
static functions
*/
static void _TaskToWait(uint16_t semID, taskCB_t *ptcb)
{
    SemCB_t *psemcb = &SEMTbl[semID];
    /* suspend thread */
    task_suspend(ptcb);

    switch (psemcb->sortType)
    {
    case FIFO:
        list_insert_before((list_t *)psemcb, (list_t*)ptcb);
        break;

    case PRIO:
        {
            list_t *plist = psemcb->node.next;
            while(plist != (list_t *)psemcb) {
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

static void _WaitTaskToRdy(uint16_t id)
{
    SemCB_t *psemcb = &SEMTbl[id];
    
    if (list_isempty((list_t*)psemcb))
        return;
    
    taskCB_t *ptcb = (taskCB_t *)psemcb->node.next;
    if(ptcb == getCurrentTask()){
	    ptcb->state = TASK_RUNNING;
	} else {
        task_resume(ptcb);
    }
}

static err_t _AllWaitTaskToRdy(uint16_t id)
{
    SemCB_t *psemcb = &SEMTbl[id];
    taskCB_t *ptcb;
    reg_t lock_status;
    uint8_t need_schedule_here = 0;
    
    while(!list_isempty((list_t*)psemcb)) {
        lock_status = spin_lock();
        //get next task
        ptcb = (taskCB_t*) psemcb->node.next;
        list_remove((list_t*)ptcb);
        ptcb->returnMsg = E_TIMEOUT;
        //set error for return task???
        //to do
        task_resume(ptcb);
        spin_unlock(lock_status);
        need_schedule_here=1;
    }
    return need_schedule_here;
}


/***
 interface 
*/
err_t createSem(uint16_t initCnt,uint16_t maxCnt,uint8_t sortType)
{
    reg_t lock_status;
    lock_status = spin_lock();
    for (int i=0; i<MAX_SEM;i++) {
        int mapIndex = i / MAP_SIZE;
        int mapOffset = i % MAP_SIZE;
        if ((SemMap[mapIndex] & (1<< mapOffset)) == 0)
        {
            SemMap[mapIndex] |= (1<< mapOffset);
            SEMTbl[i].id = i;
            SEMTbl[i].semCounter = initCnt;
            SEMTbl[i].initialCounter = maxCnt;
            SEMTbl[i].sortType = sortType;
            list_init((list_t*)&SEMTbl[i].node);
            return i;
        }
    }
    spin_unlock(lock_status);
    return E_CREATE_FAIL;
}

 
void delSem(uint16_t semID)
{   
    /* free semaphore control block */
    int mapIndex = semID / MAP_SIZE;
    int mapOffset = semID % MAP_SIZE;
    reg_t lock_status = spin_lock();
    SemMap[mapIndex] &=~(1<<mapOffset);   
    spin_unlock(lock_status);  
    /* wakeup all suspended threads */
    if (_AllWaitTaskToRdy(semID))
        task_yield();                             
}

/*
timeout = 0, try, -1: for ever
*/
err_t sem_take(uint16_t semID, int timeout){
    SemCB_t *psemcb = &SEMTbl[semID];
    taskCB_t *ptcb;
    reg_t lock_status=spin_lock();

    if (psemcb->semCounter > 0) {
        /* semaphore is available */
        psemcb->semCounter --;
        spin_unlock(lock_status);
    } else {
        /* no waiting, return with timeout */
        if (timeout == 0) {
            spin_unlock(lock_status);
            return E_TIMEOUT;
        }
        //return OK
        ptcb = getCurrentTask();
        ptcb->returnMsg = E_OK;
        _TaskToWait(semID, ptcb);
        /* has waiting time, start thread timer */      
        if (timeout > 0) {
            //waiting in delayList
            setCurTimerCnt(ptcb->timer->timerID,timeout, timeout);
            startTimer(ptcb->timer->timerID);
        }  
        spin_unlock(lock_status);
        task_yield();

        //wake up here if has a value or timeout
        if (ptcb->returnMsg != E_OK) { //mean timeout, task remove from sem queue when taskTimeout
            return ptcb->returnMsg;
        } else { //wake up from sem queue
            stopTimer(ptcb->timer->timerID); //remove timer from delayList
            return E_OK;
        }
    }
    return E_OK;
}

err_t sem_trytake(uint16_t semID) {
    return sem_take(semID, 0);
}


err_t sem_release(uint16_t semID)
{
    SemCB_t *psemcb=&SEMTbl[semID];
    reg_t  need_schedule_sem;
    
    need_schedule_sem = 0;
    reg_t lock_status = spin_lock();

    if (!list_isempty((list_t*)psemcb)) {
        /* resume the suspended task */
        _WaitTaskToRdy(semID);
        need_schedule_sem = 1;
    } else {
        if (psemcb->semCounter < MAX_SEM_VALUE) 
            psemcb->semCounter++;
        else {
            spin_unlock(lock_status);
            return E_FULL;
        }
    }
    spin_unlock(lock_status);
    if (need_schedule_sem) 
        task_yield();

    return E_OK;
}    


