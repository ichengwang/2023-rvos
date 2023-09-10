#include "os.h"

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	char i=0;
	while (1) {
		kprintf("i=%d\n",i);
		i++;
	}; // stop here!
}

