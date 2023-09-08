#include "osConfig.h"
#include "types.h"
#include "os.h"

/*!< Table use to save TCB pointer.              */
taskCB_t    TCBTbl[MAX_USER_TASKS+SYS_TASK_NUM] = {{0}};

/*!< The stack of IDLE task.                     */
uint32_t   idle_stk[SYS_STACK_SIZE] = {0};

taskCB_t *    FreeTCB            = NULL;         /*!< pointer to free TCB                        */
taskCB_t *    TCBRdy[PRIO_LEVEL]= {NULL};        /*!< READY list.                 */
taskCB_t *    TCBClose           = NULL;
taskCB_t *    TCBNext            = NULL;        /*!< Poniter to task that next scheduled by OS  */
taskCB_t *    TCBRunning         = NULL;        /*!< Pointer to TCB that current running task.  */
uint64_t      ticks = 0;     /*!< The counter of system tick.                */


/**
 *******************************************************************************
 * @brief      Create a TCB list.	  
 *******************************************************************************
 */
void InitTCBList(void)
{	
    uint16_t  i;
    taskCB_t * ptcb1;
    taskCB_t * ptcb0;
    
    FreeTCB = &TCBTbl[0];	/* Build the free TCB list            */
    FreeTCB->node.prev =  NULL;
    FreeTCB->node.next = NULL;

    FreeTCB->state = TASK_INIT;
    FreeTCB->taskID = 0;
    for(i=1;i< MAX_USER_TASKS+SYS_TASK_NUM;i++ )
    {
        ptcb1->taskID    = i;
        ptcb1->state     = TASK_INIT;
        ptcb1->node.next = NULL;
        ptcb0 = ptcb1-1;
        ptcb1->node.prev = (list_t *)ptcb0;
        ptcb0->node.next = (list_t *)ptcb1;
        ptcb1++;
    }	
}

static taskCB_t * _getFreeTCB(void)
{
    taskCB_t * ptcb;

    spin_lock();
    if(FreeTCB == NULL)                 /* Is there no free TCB               */
    {
        spin_unlock();                  /* Yes,unlock schedule                */
        return NULL;                    /* Error return                       */
    }	
    ptcb    = FreeTCB;          /* Yes,assgin free TCB for this task  */    
    /* Set next item as the head of free TCB list                     */
    FreeTCB = ptcb->node.next; 
    ptcb->node.next = NULL;
    ptcb->node.prev = NULL;
    spin_unlock();
    return ptcb;        
}


taskCB_t * task_create(const char *name,
                  void (*taskFunc)(void *parameter),
                  void       *parameter,
                  uint32_t    stack_size,
                  uint16_t    priority,
                  uint16_t    tick)
{
    taskCB_t *ptcb = _getFreeTCB();
    if (ptcb == NULL) { return NULL; }

    void *stack_start;
    stack_start = (void *)malloc(stack_size);
    if (stack_start == NULL)
    {
        /* allocate stack failure */
        return NULL;
    }

    memcpy(ptcb->name, name, sizeof(ptcb->name));
    ptcb->entry = (void *)taskFunc;
    ptcb->parameter = parameter;

    /* stack init */
    ptcb->stack_addr = stack_start;
    ptcb->stack_size = stack_size;

    /* init thread stack */
    memset(ptcb->stack_addr, '0', ptcb->stack_size);
    
    ptcb->init_priority    = priority;
    ptcb->current_priority = priority;

    /* tick init */
    ptcb->init_tick      = tick;
    ptcb->remaining_tick = tick;

    ptcb->tmrID = create_timer(TMR_TYPE_ONE_SHOT, 0, 0, task_timeout, ptcb, parameter);
    return ptcb;
}	

void task_startup(taskCB_t * ptcb)
{
    /* set current priority to initialize priority */
    ptcb->current_priority = ptcb->init_priority;

    /* change thread stat */
    ptcb->state = TASK_SUSPEND;
    /* then resume it */
    task_resume(ptcb);
    if (get_running_task() != NULL)
    {
        /* do a scheduling */
        schedule();
    }
}


err_t task_resume(taskCB_t *ptcb)
{
    if (ptcb->state != TASK_SUSPEND)
    {
        return ERROR;
    }

    //timer_stop(&ptcb->timer);
    spin_lock(); 
    /* remove from suspend list */
    list_remove((list_t*)ptcb);
    list_insert_before((list_t*)&TCBRdy[ptcb->current_priority], (list_t*)ptcb); 
    spin_unlock();

    return OK;

}

err_t task_suspend(taskCB_t * ptcb)
{
    if (ptcb->state != TASK_READY)
    {
        return ERROR;
    }

    spin_lock();
    /* change thread stat */
    list_remove((list_t*)ptcb);
    ptcb->state = TASK_SUSPEND;

    /* stop thread timer anyway */
    //timer_stop(&(ptcb->timer));
    spin_unlock();
    return OK;
}


/**
 * This function is the timeout function for thread, normally which is invoked
 * when task is timeout to wait some resource.
 *
 * @param parameter the parameter of task timeout function
 */
void task_timeout(void *parameter)
{
    taskCB_t *ptcb;
    ptcb = (taskCB_t *)parameter;
    /* set error number */
    ptcb->error = TIMEOUT;
    spin_lock();
    /* remove from suspend list */
    list_remove((list_t*)ptcb);
    /* insert to schedule ready list */
    list_insert_before((list_t*)&TCBRdy[ptcb->current_priority], (list_t*)ptcb);
    spin_unlock();
    /* do schedule */
    schedule();
}



err_t task_yield(void)
{
    taskCB_t *ptcb;

    spin_lock();
    /* set to current thread */
    ptcb = TCBRunning;

    /* if the task stat is READY and on ready queue list */
    if (ptcb->state == TASK_READY)
    {
        /* remove task from task list */
        list_remove((list_t*)ptcb);
        /* put task to end of ready queue */
        list_insert_before((list_t*)&TCBRdy[ptcb->current_priority], (list_t*)ptcb);
        spin_unlock();
        schedule();
        return OK;
    }
    spin_unlock();
    return OK;
}
