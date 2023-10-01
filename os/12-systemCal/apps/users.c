#include "os.h"
#define DELAY 20000000L
void shareRoutine(int data, int timeout);

int shared_var = 500;

spinlock_t lock;

static void myDelay(int Delay) {
    for (unsigned long long i=0;i<Delay;i++);
}

void user_task0(void *p)
{
	DEBUG("Task 0: Created!\n");
	uint32_t i=0;
	while (1){
		DEBUG("				Task 0: dalay...%d \n", i);
		delay(2);
		DEBUG("				Task 0: wakeup...%d \n", i++);		
        shareRoutine(0,10);
		myDelay(DELAY);
        DEBUG("				return Task 0 \n");
	}
}

void user_task1(void *p)
{
	DEBUG("Task 1: Created!\n");
	uint32_t i=0;
	while (1){
		DEBUG("			Task 1: delay...%d \n", i++);
		delay(2);
		DEBUG("				Task 1: wakeup...%d \n", i++);		
        shareRoutine(1,3);
		myDelay(DELAY);
        DEBUG("				return Task 1 \n");
	}
}

void user_task2(void)
{
	DEBUG("Task2: Created!\n");
	while (1)
	{
		for (int i = 0; i < 50; i++)
		{
			lock_acquire(&lock);
			shared_var++;
			lock_free(&lock);
			delay(2);
		}
		DEBUG("The value of shared_var is: %d \n", shared_var);
	}
}

void user_task3(void)
{
	DEBUG("Task3: Created!\n");
	while (1)
	{
		DEBUG("Trying to get the lock... \n");
		lock_acquire(&lock);
		DEBUG("Get the lock!\n");
		lock_free(&lock);
		delay(5);
	}
}

void user_task4(void)
{
	DEBUG("Task4: Created!\n");
	unsigned int hid = -1;
	
	/*
	 * if syscall is supported, this will trigger exception, 
	 * code = 2 (Illegal instruction)
	 */
	
	// perform system call from the user mode
	int ret = -1;

	while (1)
	{	
		DEBUG("Task4: Running...\n");
		ret = gethid(&hid);
    	if (!ret) 
			DEBUG("system call returned!, hart id is %d\n", hid);
    	else 
        	DEBUG("gethid() failed, return: %d\n", ret);
		delay(10);
	}
}


void loadTasks(void)
{
    taskCB_t *task0, *task1, *task2, *task3, *task4;
    task0 = task_create("task0", user_task0, NULL, 1024, 9,200);
    task1 = task_create("task1", user_task1, NULL, 1024, 11,200);
    task2 = task_create("task2", user_task2, NULL, 1024, 11,200);
    task3 = task_create("task3", user_task3, NULL, 1024, 10,200);
    task4 = task_create("task4", user_task4, NULL, 1024, 9,200);
    ipcTestInit();
	
	task_startup(task0);
    task_startup(task1);
    task_startup(task2);
    task_startup(task3);
	
	task_startup(task4);
}