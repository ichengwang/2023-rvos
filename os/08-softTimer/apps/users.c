#include "os.h"
#define TASK_DELAY
void user_task0(void *p)
{
	uart_puts("Task 0: Created!\n");

	while (1){
		uart_puts("Task 0: Running... \n");
        taskDelay(3);
		task_yield();
		uart_puts("return Task 0 \n");
	}
}

void user_task1(void *p)
{
	uart_puts("Task 1: Created!\n");
	while (1) {
		uart_puts("Task 1: Running... \n");
        taskDelay(10);
		task_yield();
        uart_puts("return Task 1 \n");
	}
}

void user_task2(void *p)
{
	uart_puts("Task 2: Created!\n");
	while (1) {
		uart_puts("Task 2: Running... \n");
        taskDelay(4);
		task_yield();
        uart_puts("return Task 2 \n");
	}
}

void timeOut1(void *p) 
{
	kprintf("Timer call back function 1\n");
}
void timeOut2(void *p) 
{
	kprintf("Timer call back function 2\n");
}
void timeOut3(void *p) 
{
	kprintf("Timer call back function 3\n");
}

void loadTasks(void)
{
#ifdef TASK_DELAY
    taskCB_t *task0, *task1, *task2;
    task0 = task_create("task0", user_task0, NULL, 1024, 11,5000);
    task1 = task_create("task1", user_task1, NULL, 1024, 11,5000);
    task2 = task_create("task2", user_task2, NULL, 1024, 11,5000);
    task_startup(task0);
    task_startup(task1);
    task_startup(task2);
#else
	int timerNum=1;
	uint16_t TimerID1=createTimer(TMR_ONE_SHOT, 10, 10, timeOut1, &timerNum);
	
	timerNum=2;
	uint16_t TimerID2=createTimer(TMR_ONE_SHOT, 3, 3, timeOut2, &timerNum);

	timerNum=3;
	uint16_t TimerID3=createTimer(TMR_PERIOD, 4, 4, timeOut3,&timerNum);
	startTimer(TimerID1);
	startTimer(TimerID2);
	startTimer(TimerID3);
#endif
}