#include "os.h"
#define DELAY 20000000L

static void myDelay(int Delay) {
    for (unsigned long long i=0;i<Delay;i++);
}

void user_task0(void *p)
{
	uart_puts("Task 0: Created!\n");
	uint32_t i=0;
	while (1){
		kprintf("				Task 0: Running...%d \n", i++);
        taskDelay(3);
		myDelay(DELAY);
        uart_puts("				return Task 0 \n");
	}
}

void user_task1(void *p)
{
	uart_puts("Task 1: Created!\n");
	uint32_t i=0;
	while (1){
		kprintf("			Task 1: Running...%d \n", i++);
		taskDelay(1);
		myDelay(DELAY);
        uart_puts("				return Task 1 \n");
	}
}

void user_task2(void *p)
{
	uart_puts("Task 2: Created!\n");
	uint32_t i=0;
	while (1){
		uint32_t tick=getTicks()&0x4;
		if (tick==0)
			kprintf("Task 2: Running...%d \n", i++);
		task_yield();
        if (tick%3==0)
			uart_puts("return Task 2 \n");
	}
}

void user_task3(void *p)
{
	uart_puts("Task 3: Created!\n");
	uint32_t i=0;
	while (1){
		uint32_t tick=getTicks()& 0x4;
		if (tick==0)
			kprintf("Task 3: Running...%d \n", i++);
		task_yield();
        if (tick%3==0)
			uart_puts("return Task 3 \n");
	}
}


void loadTasks(void)
{
    taskCB_t *task0, *task1, *task2, *task3;
    task0 = task_create("task0", user_task0, NULL, 1024, 11,200);
    task1 = task_create("task1", user_task1, NULL, 1024, 11,200);
    task2 = task_create("task2", user_task2, NULL, 1024, 12,200);
    task3 = task_create("task2", user_task2, NULL, 1024, 12,200);
    task_startup(task0);
    task_startup(task1);
    task_startup(task2);
    task_startup(task3);
}