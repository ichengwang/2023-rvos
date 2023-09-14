#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context *next);
extern taskCB_t TCBRdy;
extern taskCB_t * TCBRunning; 

void sched_init()
{
	w_mscratch(0);
}

void schedule()
{
	//get next task ctx_t in readyQueue
	taskCB_t *nextTask= (taskCB_t*)TCBRdy.node.next;
	ctx_t *next = &nextTask->ctx;
	list_remove((list_t*)nextTask);
	//current task into ready queue
	if (TCBRunning != NULL){//kernel
		taskCB_t *currentTask = TCBRunning;
		currentTask->state = TASK_READY;
		list_insert_before((list_t*)&TCBRdy, (list_t*)currentTask);
	} 
	//
	TCBRunning = nextTask;
	nextTask->state = TASK_RUNNING;
	switch_to(next);
}

