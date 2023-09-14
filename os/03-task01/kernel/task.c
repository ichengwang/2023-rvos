#include "os.h"

/*!< Table use to save TCB pointer.              */
taskCB_t    TCBTbl[256];

taskCB_t *    TCBRunning         = NULL;        /*!< Pointer to TCB that current running task.  */
taskCB_t      TCBRdy;        /*!< READY list.                 */

void readyQ_init() {
    list_init((list_t *)&TCBRdy);
}

taskCB_t * getNewTCB(uint8_t index) {
    return &TCBTbl[index];
}
err_t task_init(taskCB_t *ptcb, const char *name,
                  void (*taskFunc)(void *parameter),
                  void       *parameter,
                  uint32_t    stack_size,
                  uint16_t    priority)
{

    void *stack_start;
    stack_start = (void *)malloc(stack_size);
    if (stack_start == NULL)
    {
        /* allocate stack failure */
        return ERROR;
    }

    memcpy(ptcb->name, name, sizeof(ptcb->name));
    ptcb->entry = (void *)taskFunc;
    ptcb->parameter = parameter;

    /* init thread stack */
    memset(ptcb->stack_addr, 0, ptcb->stack_size);
    
    ptcb->ctx.ra = (reg_t)taskFunc;
    ptcb->ctx.sp = (reg_t)(stack_start + stack_size);
    
    ptcb->priority    = priority;
    list_init((list_t*)ptcb);
    return OK;
}	

void task_startup(taskCB_t * ptcb)
{

    /* change thread stat */
    ptcb->state = TASK_SUSPEND;
    /* then resume it */
    task_resume(ptcb);
}


err_t task_resume(taskCB_t *ptcb)
{
    if (ptcb->state != TASK_SUSPEND)
    {
        return ERROR;
    }

    /* remove from suspend list */
    list_remove((list_t*)ptcb);
    list_insert_before((list_t*)&TCBRdy, (list_t*)ptcb); 
    return OK;
}

err_t task_yield(void)
{
    taskCB_t *ptcb;

    /* set to current task */
    ptcb = TCBRunning;
    if (ptcb->state == TASK_READY) {
        /* remove task from task list */
        list_remove((list_t*)ptcb);
        /* put task to end of ready queue */
        list_insert_before((list_t*)&TCBRdy, (list_t*)ptcb);
    }
    schedule();
    return OK;
}
