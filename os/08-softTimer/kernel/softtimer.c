#include "os.h"

/*!< Table use to save TCB pointer.              */
timerCB_t    TIMCBTbl[MAX_TIMER]={{0}};
uint32_t     TimerMap[MAX_TIMER/32];
timerCB_t     *TimerList = NULL;


InsertTimerList();
removeTimerList();
createTimer();
startTimer();
stopTimer();
delTimer();
getCurTimerCnt();
setCurTimerCnt();
