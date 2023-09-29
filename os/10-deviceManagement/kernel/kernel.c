#include "os.h"

/*
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 */
extern void uart_init(void);
extern void page_init(void);


void start_kernel(void)
{

	
	disableINT();	//disable all interrupt, util before switch_first
	page_init();

	InitTCBList();
	readyQ_init();
	deviceList_init();
	drivers_init();
	trap_init();
	timer_init();
	softTimer_init(); //08
	idleTask_init(); //08
	loadTasks();
	sched_init();	
	
	
	schedule();
	DEBUG("Would not go here!\n");
	while (1) {}; // stop here!	
}

