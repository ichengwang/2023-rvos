#ifndef __OS_H__
#define __OS_H__

#include <stdarg.h>
#include "osConfig.h"
#include "types.h"
#include "platform.h"
#include "riscv.h"
#include "libs.h"
#include "osTask.h"
#include "osTimer.h"
#include "osSem.h"



/* uart */
extern int uart_putc(char ch);
extern void uart_puts(char *s);
extern int uart_getc(void);

/* printf */
extern int  printf(const char* s, ...);
extern void panic(char *s);

/* memory management */
extern void *page_alloc(int npages);
extern void page_free(void *p);


#endif /* __OS_H__ */
