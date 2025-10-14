#include "os.h"

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	// 作業二 demo
	int i;
	char str[100];
	kprintf("input a number & string\n");
	int res = kscanf("%d %s", &i, str);
	kprintf("i = %d\n",i);
	kprintf("str = %s\n",str);
	kprintf("res = %d\n",res);

	while(1) 
	{
		char c = uart_getc();
		if(c == 13) uart_putc('\n');
		else if(c == 127) uart_puts("\b \b");
		else uart_putc(c);
	}
}

