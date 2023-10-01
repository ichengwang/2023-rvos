#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>

/* uart */
int uart_putc(char ch);
void uart_puts(char *s);

/* printf */
int kprintf(const char* s, ...);
void panic(char *s);

/* memory management */
void page_init();
void *malloc(size_t size);
void free(void *p);

#endif /* __OS_H__ */
