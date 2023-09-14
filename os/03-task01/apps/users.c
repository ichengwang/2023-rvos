#include "os.h"

#define DELAY 40000



static void myDelay(int Delay) {
    for (int i=0;i<Delay;i++);
}

void user_task0(void *p)
{
	uart_puts("Task 0: Created!\n");

	while (1){
		uart_puts("Task 0: Running... \n");
        myDelay(DELAY);
        task_yield();
	}
}

void user_task1(void *p)
{
	uart_puts("Task 1: Created!\n");
	while (1) {
		uart_puts("Task 1: Running... \n");
		myDelay(DELAY);
        task_yield();
	}
}

void loadTasks(void)
{
    taskCB_t *task0, *task1;
    task0 = getNewTCB(0);
    task1 = getNewTCB(1);
    task_init(task0, "task0", user_task0, NULL, 1024, 10);
    task_init(task1, "task1", user_task1, NULL, 1024, 10);
    task_startup(task0);
    task_startup(task1);
}
