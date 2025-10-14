#include "os.h"

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	int i=10;
	kprintf("i=%d\n",i);
	while(1) 
	{
		char c = uart_getc();
		if(c == 13) uart_putc('\n');
		else if(c == 127) uart_puts("\b \b");
		else uart_putc(c);
	}
}

