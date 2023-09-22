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
		i++;
		kprintf("Task 0: Running... %d\n",i);
        myDelay(DELAY);
		task_yield();
		uart_puts("return Task 0 \n");
	}
}

void user_task1(void *p)
{
	uart_puts("Task 1: Created!\n");
	uint32_t i=0;
	while (1){
		i++;
		kprintf("Task 0: Running... %d\n",i);
        myDelay(DELAY);
		task_yield();
        uart_puts("return Task 1 \n");
	}
}

void user_task2(void *p)
{
	uart_puts("Task 2: Created!\n");
	uint32_t i=0;
	while (1){
		i++;
		kprintf("Task 0: Running... %d\n",i);
 		myDelay(DELAY);
		task_yield();
        uart_puts("return Task 2 \n");
	}
}

void loadTasks(void)
{
    taskCB_t *task0, *task1, *task2;
    task0 = task_create("task0", user_task0, NULL, 1024, 11,5000);
    task1 = task_create("task1", user_task1, NULL, 1024, 11,5000);
    task2 = task_create("task2", user_task2, NULL, 1024, 11,5000);
    task_startup(task0);
    task_startup(task1);
    task_startup(task2);
}