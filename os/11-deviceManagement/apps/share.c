#include "os.h"

int sR_sem;
void ipcTestInit() {
	sR_sem=(int) createSem(1,1,FIFO);
}

void shareRoutine(int data, int timeout)
{
	DEBUG("Enter shareRoutine\n");
	if (sem_take(sR_sem, timeout)!=E_OK) 
	{
		DEBUG("sem Taking timeout\n");
	}
	//waiting for ever if wait 
	/*   this section is critical */
	DEBUG("Task %s: Enter critical\n", getCurrentTask()->name);
	DEBUG("waiting in SEM\n");
    taskDelay(5);
	DEBUG("data is = %d\n",data);
	/* critical to here*/
	DEBUG("Task %s: Release sem\n",getCurrentTask()->name);
	sem_release(sR_sem);
}
