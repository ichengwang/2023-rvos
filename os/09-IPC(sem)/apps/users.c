#include "os.h"
#define DELAY 20000000L
void shareRoutine(int data, int timeout);

static void myDelay(int Delay) {
    for (unsigned long long i=0;i<Delay;i++);
}

void user_task0(void *p)
{
	uart_puts("Task 0: Created!\n");
	uint32_t i=0;
	while (1){
		kprintf("				Task 0: dalay...%d \n", i);
		taskDelay(2);
		kprintf("				Task 0: wakeup...%d \n", i++);		
        shareRoutine(0,-1);
		myDelay(DELAY);
        uart_puts("				return Task 0 \n");
	}
}

void user_task1(void *p)
{
	uart_puts("Task 1: Created!\n");
	uint32_t i=0;
	while (1){
		kprintf("			Task 1: delay...%d \n", i++);
		taskDelay(2);
		kprintf("				Task 1: wakeup...%d \n", i++);		
        shareRoutine(1,-1);
		myDelay(DELAY);
        uart_puts("				return Task 1 \n");
	}
}

void user_task2(void *p)
{
	uart_puts("Task 2: Created!\n");
	uint32_t i=0;
	while (1){
		kprintf("			Task 2: delay...%d \n", i++);
		taskDelay(2);
		kprintf("				Task 2: wakeup...%d \n", i++);		
        shareRoutine(2,-1);
		myDelay(DELAY);
        uart_puts("				return Task 2 \n");
	}
}

void user_task3(void *p)
{
	uart_puts("Task 3: Created!\n");
	uint32_t i=0;
	while (1){
		kprintf("			Task 3: delay...%d \n", i++);
		taskDelay(2);
		kprintf("				Task 3: wakeup...%d \n", i++);		
        shareRoutine(3,-1);
		myDelay(DELAY);
        uart_puts("				return Task 3 \n");
	}
}


void loadTasks(void)
{
    taskCB_t *task0, *task1, *task2, *task3;
    task0 = task_create("task0", user_task0, NULL, 1024, 9,200);
    task1 = task_create("task1", user_task1, NULL, 1024, 11,200);
    task2 = task_create("task2", user_task2, NULL, 1024, 11,200);
    task3 = task_create("task3", user_task3, NULL, 1024, 10,200);
    ipcTestInit();
	task_startup(task0);
    task_startup(task1);
    task_startup(task2);
    task_startup(task3);
}