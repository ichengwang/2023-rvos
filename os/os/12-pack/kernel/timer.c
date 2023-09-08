#include "os.h"

timer_t         TmrTbl[CFG_MAX_TMR]= {{0}}; /*!< Table which save timer control block.*/
timer_t      *  TmrList     = NULL;         /*!< The header of the Timer list.      */
uint8_t         TmrBitmap[CFG_NUM_TMR] = 0; /*!< Timer ID container.                  */


static void InsertTmrList(uint32_t tmrID)
{
    timer_t * pTmr;
    sint32_t deltaTicks;
    uint32_t tmrCnt;
    tmrCnt = TmrTbl[tmrID].tmrCnt;      /* Get timer time                     */
    
    if(tmrCnt == 0)                     /* Is timer time==0?                  */
    {
        return;                         /* Do nothing,return                  */
    }
    
    spin_lock();
    if(TmrList == NULL)                 /* Is no item in timer list?          */
    {
        TmrList = &TmrTbl[tmrID];       /* Yes,set this as first item         */
    }
    else                  /* No,find correct place ,and insert inserted timer */
    {								    
      	pTmr       = TmrList; 
      	deltaTicks = tmrCnt;            /* Get timer tick                     */
      	
      	/* find correct place */
      	while(pTmr != NULL)
      	{				    
            deltaTicks = deltaTicks - pTmr->tmrCnt; /* Get ticks with previous item       */
            if(deltaTicks < 0)          /* Is delta ticks<0?                  */  
            {	
                /* Yes,get correct place */
                if(pTmr->tmrPrev!= NULL)/* Is head item of timer list?        */
                {	
                    /* No,insert into */
                    pTmr->tmrPrev->tmrNext = &TmrTbl[tmrID]; 
                    TmrTbl[tmrID].tmrPrev  = pTmr->tmrPrev;
                    TmrTbl[tmrID].tmrNext  = pTmr;
                    pTmr->tmrPrev          = &TmrTbl[tmrID];
                }
                else                    /* Yes,set task as first item         */ 	
                {
                    TmrTbl[tmrID].tmrNext = TmrList;
                    TmrList->tmrPrev      = &TmrTbl[tmrID];
                    TmrList               = &TmrTbl[tmrID];
                }
                TmrTbl[tmrID].tmrCnt            = TmrTbl[tmrID].tmrNext->tmrCnt+deltaTicks;
                TmrTbl[tmrID].tmrNext->tmrCnt  -= TmrTbl[tmrID].tmrCnt; 
                break;	
            }
            /* Is last item in list? */									
            else if((deltaTicks >= 0) && (pTmr->tmrNext == NULL))
            {	
                /* Yes,insert into */
                TmrTbl[tmrID].tmrPrev = pTmr;
                pTmr->tmrNext         = &TmrTbl[tmrID];	
                TmrTbl[tmrID].tmrCnt  = deltaTicks;
                break;	
            }
            pTmr = pTmr->tmrNext;       /* Get the next item in timer list    */	
      	}
    }
    spin_unlock();
}


static void RemoveTmrList(uint32_t tmrID)
{
    timer_t * pTmr;
    
    pTmr = &TmrTbl[tmrID];
    
    spin_lock();
    
    /* Is there only one item in timer list?                                  */
    if((pTmr->tmrPrev == NULL) && (pTmr->tmrNext == NULL))
    {		
        TmrList = NULL;                 /* Yes,set timer list as Co_NULL         */
    }
    else if(pTmr->tmrPrev == NULL)      /* Is the first item in timer list?   */
    {   /* Yes,remove timer from list,and reset timer list                    */
        TmrList  = pTmr->tmrNext;
        TmrList->tmrPrev = NULL;
        pTmr->tmrNext->tmrCnt += pTmr->tmrCnt;
        pTmr->tmrNext    = NULL;
    }
    else if(pTmr->tmrNext == NULL)      /* Is the last item in timer list?    */
    {
        /* Yes,remove timer form list */
        pTmr->tmrPrev->tmrNext = NULL;
        pTmr->tmrPrev = NULL;
    }
    else                                /* No, remove timer from list         */
    {
        pTmr->tmrPrev->tmrNext  =  pTmr->tmrNext;
        pTmr->tmrNext->tmrPrev  =  pTmr->tmrPrev;
        pTmr->tmrNext->tmrCnt  += pTmr->tmrCnt;
        pTmr->tmrNext = NULL;
        pTmr->tmrPrev = NULL;
    }
    spin_unlock();
}


int create_timer(uint8_t tmrType, uint32_t tmrCnt, uint32_t tmrReload, void (*taskFunc)(void *parameter), void *parameter)
{
    uint32_t i;
    uint32_t index;
    uint8_t offset;
    
    spin_lock();
    for(i = 0; i < CFG_MAX_TMR; i++)
    {
        index = i / 8;
        offset = i % 8;
        if((TmrBitmap[index] & (1 << offset)) == 0) /* Is free timer ID?                */
        {
            TmrBitmap[index] |= (1<<offset);        /* Yes,assign ID to this timer      */
            spin_unlock();
            TmrTbl[i].tmrID     = i;      /* Initialize timer as user set     */
            TmrTbl[i].tmrType   = tmrType;	
            TmrTbl[i].tmrState  = TMR_STATE_STOPPED;
            TmrTbl[i].tmrCnt    = tmrCnt;
            TmrTbl[i].tmrReload	= tmrReload;
            TmrTbl[i].tmrCallBack = callbackfunc;
            TmrTbl[i].paramater = paramater;
            TmrTbl[i].node->prev   = NULL;
            TmrTbl[i].node->next   = NULL;
            return i;                     /* Return timer ID                  */
        }
    }
    spin_unlock();                      /* Unlock schedule                  */
    return -1;                        /* Error return                     */
}


StatusType start_timer(uint32_t tmrID)
{
    
    if(TmrTbl[tmrID].tmrState == TMR_STATE_RUNNING)   /* Is timer running?    */
    {
        return E_OK;                              /* Yes,do nothing,return OK */
    }
    
    /* No,set timer status as TMR_STATE_RUNNING */
    TmrTbl[tmrID].tmrState = TMR_STATE_RUNNING; 
    InsertTmrList(tmrID);               /* Insert this timer into timer list  */
    return E_OK;                        /* Return OK                          */
}



StatusType stop_timer(uint32_t tmrID)
{	   
    if(TmrTbl[tmrID].tmrState == TMR_STATE_STOPPED)/* Does timer stop running?*/
    {
        return E_OK;                    /* Yes,do nothing,return OK           */
    }
    RemoveTmrList(tmrID);             /* No,remove this timer from timer list */
    
    /* Set timer status as TMR_STATE_STOPPED  */
    TmrTbl[tmrID].tmrState = TMR_STATE_STOPPED;	
    return E_OK;                        /* Return OK                          */
}


StatusType delete_timer(uint32_t tmrID)
{	
    if(TmrTbl[tmrID].tmrState == TMR_STATE_RUNNING) /* Is timer running?      */
    {
        RemoveTmrList(tmrID);         /* Yes,remove this timer from timer list*/
    }
    uint32_t index;
    uint8_t offset;
    index = tmrID / 8;
    offset = tmrID % 8;
    TmrBitmap[index] &=~(1<<offset);        /* Release resource that this timer hold*/
    return E_OK;                      /* Return OK                            */
}

 
uint32_t getCurTmrCnt(uint32_t tmrID, StatusType* perr)
{
    *perr = E_OK;
    return TmrTbl[tmrID].tmrCnt;        /* Return timer counter               */
}


StatusType SetTmrCnt(uint32_t tmrID, uint32_t tmrCnt, uint32_t tmrReload)
{
    TmrTbl[tmrID].tmrCnt    = tmrCnt; /* Reset timer counter and reload value */
    TmrTbl[tmrID].tmrReload = tmrReload;
    								
    if(TmrTbl[tmrID].tmrState == TMR_STATE_RUNNING)   /* Is timer running?    */
    {
        RemoveTmrList(tmrID);           /* Yes,reorder timer in timer list    */
        InsertTmrList(tmrID);	
    }
    return E_OK;                        /* Return OK                          */
}


void TmrDispose(void)
{
    timer_t *	pTmr;
    
    pTmr = TmrList;                     /* Get first item of timer list       */
    while((pTmr != NULL) && (pTmr->tmrCnt == 0) )
    {	
        if(pTmr->tmrType == TMR_TYPE_ONE_SHOT)    /* Is a One-shot timer?     */
        {
            /* Yes,remove this timer from timer list                          */
            RemoveTmrList(pTmr->tmrID);
            
            /* Set timer status as TMR_STATE_STOPPED                          */
            pTmr->tmrState = TMR_STATE_STOPPED;
            (pTmr->tmrCallBack)(pTmr->parameter);          /* Call timer callback function   */
        }
        else if(pTmr->tmrType == TMR_TYPE_PERIODIC)   /* Is a periodic timer? */
        {
            /* Yes,remove this timer from timer list                          */
            RemoveTmrList(pTmr->tmrID); 
            pTmr->tmrCnt = pTmr->tmrReload;   /* Reset timer tick             */
            InsertTmrList(pTmr->tmrID);       /* Insert timer into timer list */
            (pTmr->tmrCallBack)(pTmr->parameter);            /* Call timer callback function */
        }
        pTmr = TmrList;	                      /* Get first item of timer list */
    }
}

