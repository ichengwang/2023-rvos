#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "config.h"
#include "platform.h"
#include "task.h"
#include "list.h"
#include "riscv.h"
#include "softtimer.h"

#include <stddef.h>
#include <stdarg.h>

/* uart */
extern int uart_putc(char ch);
extern void uart_puts(char *s);

/* printf */
extern int  kprintf(const char* s, ...);
extern void panic(char *s);

/* mem */
void *memset(void *ptr, int value, size_t num);
char* memcpy(void* dest,const void* src, size_t num);

/* lock */
reg_t spin_lock();
void spin_unlock(reg_t);

/* memory management */
extern void *page_alloc(int npages);
extern void page_free(void *p);
void *malloc(uint32_t size);
void free(void *);

/* task*/
void readyQ_init();
void InitTCBList(void);
taskCB_t * task_create(const char *name,
                  void (*taskFunc)(void *parameter),
                  void       *parameter,
                  uint32_t    stack_size,
                  uint16_t    priority,
                  uint32_t    ticks);
void task_startup(taskCB_t * ptcb);
err_t task_resume(taskCB_t *ptcb);
err_t task_suspend(taskCB_t * ptcb);
err_t task_yield(void);
taskCB_t *getCurrentTask();
void taskDelay(uint32_t);
err_t idleTask_init();


/* sched */
void sched_init();
void schedule();

/* trap */
void trap_init();
reg_t trap_handler(reg_t epc, reg_t cause);

/* timer */
void timer_load(int interval);
void timer_init();
void timer_handler();
uint64_t getTicks();

/*softtimer*/
void softTimer_init();
err_t createTimer(uint8_t timerType,  
                  uint32_t timerCount,
                  uint32_t timerReload,
                  void(*callback)(void *parameter),
                  void *parameter
                  );
err_t startTimer(uint16_t timerID);
err_t stopTimer(uint16_t timerID);
err_t delTimer(uint16_t timerID);
uint32_t getCurTimerCnt(uint16_t timerID);
err_t setCurTimerCnt(uint16_t timerID,
                     uint32_t timerCount,
                     uint32_t timerReload);
void timerDispose(void);

/* user*/
void loadTasks(void);
#endif /* __OS_H__ */
