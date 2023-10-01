#include "os.h"

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	int i=10;
	kprintf("i=%d\n",i);
	while(1) 
	{}
}

