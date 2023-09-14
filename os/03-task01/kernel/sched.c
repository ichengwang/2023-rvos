#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context *next);
extern taskCB_t * TCBRdy;

void sched_init()
{
	w_mscratch(0);
}

void schedule()
{
	ctx_t *next = &TCBRdy->ctx;
	//get next task ctx_t in readyQueue
	list_remove((list_t*)next);
	switch_to(next);
}

