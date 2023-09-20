#include "os.h"
#define MAP_SIZE sizeof(uint32_t)
/*!< Table use to save TCB pointer.              */
timerCB_t    TIMCBTbl[MAX_TIMERS];
uint32_t     TimerMap[MAX_TIMERS/MAP_SIZE];
timerCB_t     *TimerList;

static void insertTimerList(uint16_t timerID)
{
    timerCB_t * pTimer = &TIMCBTbl[timerID];
    timerCB_t * pTimerList;
    int deltaTicks;
    uint32_t timerCount=TIMCBTbl[timerID].timerCnt;

    if (timerCount > 0) {// if timerCount == 0 do nothing
        spin_lock();
        if (list_isempty((list_t*)TimerList)) {
            list_insert_before((list_t*)TimerList, (list_t*)pTimer);
        } else {
            pTimerList = (timerCB_t*)TimerList->node.next;
            deltaTicks = timerCount;
            /* find correct place*/
            while(pTimerList != TimerList) {
                deltaTicks -= pTimerList->timerCnt;
                if (deltaTicks < 0) {
                    /* get correct place*/
                    list_insert_before((list_t*)pTimerList, (list_t*)pTimer);
                    pTimer->timerCnt = pTimerList->timerCnt + deltaTicks;
                    pTimerList->timerCnt = deltaTicks*-1;  
                    break;
                }
                pTimerList = (timerCB_t*)pTimerList->node.next;
                if (pTimerList == TimerList) {//add to last 
                    list_insert_before((list_t*)TimerList, (list_t*)pTimer);
                    pTimer->timerCnt = deltaTicks;
                }
            }
        }
        spin_unlock();
    }
}

/*
remove from TimerList
if last node then remove directly
if not last node then remove and change timerCount
*/
static void removeTimerList(uint16_t timerID)
{
    timerCB_t * pTimer = &TIMCBTbl[timerID];
    timerCB_t * pNextTimer;
    // TimerList is critical
    spin_lock();
    list_remove((list_t*)pTimer);
    if (!list_isempty((list_t*)TimerList)) {
        pNextTimer = (timerCB_t *)(pTimer->node.next);
        if (pNextTimer != TimerList){//not last node then change timerCnt
            pNextTimer->timerCnt += pTimer->timerCnt;
        } // last node remove direct
    }
    spin_unlock();
}

/*
return timer_id or fail 
*/
err_t createTimer(uint8_t timerType,  
                  uint32_t timerCount,
                  uint32_t timerReload,
                  void(*callback)(void)
                  )
{
    spin_lock();
    for (int i=0; i<MAX_TIMERS;i++) {
        int mapIndex = i / MAP_SIZE;
        int mapOffset = i % MAP_SIZE;
        if ((TimerMap[mapIndex] & (1<< mapOffset)) == 0)
        {
            TimerMap[mapIndex] |= (1<< mapOffset);
            spin_unlock();
            TIMCBTbl[i].timerID = i;
            TIMCBTbl[i].timerType = timerType;
            TIMCBTbl[i].timerState = TMR_STOPPED;
            TIMCBTbl[i].timerReload = timerReload;
            TIMCBTbl[i].timerCnt  = timerCount;
            TIMCBTbl[i].timerCallBack = callback;
            list_init((list_t*)&TIMCBTbl[i].node);
            return i;
        }
    }
    spin_unlock();
    return E_CREATE_FAIL;
}

void softTimer_init() {
    TimerList = (timerCB_t *)malloc(sizeof(timerCB_t));
    list_init((list_t*)TimerList);
}

/*
insert to list
*/
err_t startTimer(uint16_t timerID) 
{
    if(TIMCBTbl[timerID].timerState == TMR_RUNNING)   /* Is timer running?    */
    {
        return E_OK;                              /* Yes,do nothing,return OK */
    }
    
    /* No,set timer status as TMR_RUNNING */
    TIMCBTbl[timerID].timerState = TMR_RUNNING; 
    insertTimerList(timerID);               /* Insert this timer into timer list  */
    return E_OK;                        /* Return OK                          */
}

/*
remove from list
*/
err_t stopTimer(uint16_t timerID)
{
    if(TIMCBTbl[timerID].timerState == TMR_STOPPED)/* Does timer stop running?*/
    {
        return E_OK;                    /* Yes,do nothing,return OK           */
    }
    removeTimerList(timerID);             /* No,remove this timer from timer list */
    
    /* Set timer status as TMR_STATE_STOPPED  */
    TIMCBTbl[timerID].timerState = TMR_STOPPED;	
    return E_OK;                        /* Return OK                          */
}

/*
    free a timer
*/
err_t delTimer(uint16_t timerID)
{
    if(TIMCBTbl[timerID].timerState == TMR_RUNNING) /* Is timer running?      */
    {
        removeTimerList(timerID);         /* Yes,remove this timer from timer list*/
    }
    int mapIndex = timerID / MAP_SIZE;
    int mapOffset = timerID % MAP_SIZE;
    TimerMap[mapIndex] &=~(1<<mapOffset);        /* Release resource that this timer hold*/
    return E_OK;                      /* Return OK                            */
}

uint32_t getCurTimerCnt(uint16_t timerID)
{
    return TIMCBTbl[timerID].timerCnt;
}


/*
reset timerCnt and timerReload
*/
err_t setCurTimerCnt(uint16_t timerID,
                     uint32_t timerCount,
                     uint32_t timerReload)
{
    TIMCBTbl[timerID].timerCnt    = timerCount; /* Reset timer counter and reload value */
    TIMCBTbl[timerID].timerReload = timerReload;
    								
    if(TIMCBTbl[timerID].timerState == TMR_RUNNING)   /* Is timer running?    */
    {
        removeTimerList(timerID);           /* Yes,reorder timer in timer list    */
        insertTimerList(timerID);	
    }
    return E_OK;                        /* Return OK                          */
}

/*
Timer dispose
*/
void timerDispose(void) 
{
    timerCB_t * pTimer;
    
    pTimer = (timerCB_t *)TimerList->node.next;
    while(pTimer != TimerList && pTimer->timerCnt == 0) {
        switch(pTimer->timerType) {
            case TMR_ONE_SHOT:
                removeTimerList(pTimer->timerID);
                pTimer->timerState = TMR_STOPPED;
                (pTimer->timerCallBack)();
                break;
            case TMR_PERIOD:
                removeTimerList(pTimer->timerID);
                pTimer->timerCnt = pTimer->timerReload;
                insertTimerList(pTimer->timerID);
                (pTimer->timerCallBack)();
            break;
        }
        pTimer = (timerCB_t*)TimerList->node.next;
    }
}