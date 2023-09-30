#include "os.h"

/*
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 */
extern void uart_init(void);
extern void page_init(void);


void start_kernel(void)
{

	
	disableINT();	
	page_init();

	InitTCBList();
	readyQ_init();
	plic_init();
	trap_init();
	uart_init();

	timer_init();
	softTimer_init(); //08
	idleTask_init(); //08
	loadTasks();
	sched_init();	
	
	
	schedule();
	uart_puts("Would not go here!\n");
	while (1) {}; // stop here!	
}

