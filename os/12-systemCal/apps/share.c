#include "os.h"

int sR_sem;
void ipcTestInit() {
	sR_sem=(int) createSem(1,1,FIFO);
}

void shareRoutine(int data, int timeout)
{
	uint32_t p[]={sR_sem, timeout};
	DEBUG("Enter shareRoutine\n");
	if (u_sem_take(p)!=E_OK) 
	{
		DEBUG("sem Taking timeout\n");
	}
	//waiting for ever if wait 
	/*   this section is critical */
	DEBUG("Task %s: Enter critical\n", getCurrentTask()->name);
	DEBUG("waiting in SEM\n");
    delay(5);
	DEBUG("data is = %d\n",data);
	/* critical to here*/
	DEBUG("Task %s: Release sem\n",getCurrentTask()->name);
	u_sem_release(sR_sem);
}
