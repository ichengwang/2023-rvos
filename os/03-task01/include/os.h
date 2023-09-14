#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "platform.h"
#include "task.h"
#include "list.h"
#include "riscv.h"

#include <stddef.h>
#include <stdarg.h>

/* uart */
int uart_putc(char ch);
void uart_puts(char *s);

/* printf */
int  kprintf(const char* s, ...);
void panic(char *s);

/* mem */
void *memset(void *ptr, int value, size_t num);
char* memcpy(void* dest,const void* src, size_t num); 
 

/* memory management */
void *page_alloc(int npages);
void page_free(void *p);
void *malloc(uint32_t size);
void free(void *);

/* task */
void readyQ_init();
taskCB_t * getNewTCB(uint8_t index);
err_t task_init(taskCB_t *ptcb, const char *name,
                  void (*taskFunc)(void *parameter),
                  void       *parameter,
                  uint32_t    stack_size,
                  uint16_t    priority);
void task_startup(taskCB_t * ptcb);
err_t task_resume(taskCB_t *ptcb);
err_t task_yield(void);


/* sched */
void schedule();
void sched_init();

/* user*/
void loadTasks(void);

#endif /* __OS_H__ */
