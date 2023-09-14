#include "os.h"

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	char ch;
	while (1) {
		ch = uart_getc();
		uart_putc(ch);
		if (ch==0x0D) uart_putc(0x0A);
	}; // stop here!
}

