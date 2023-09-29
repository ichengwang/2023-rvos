#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "config.h"
#include "platform.h"
#include "task.h"
#include "list.h"
#include "riscv.h"
#include "softtimer.h"
#include "ipc.h"

#include <stddef.h>
#include <stdarg.h>


#ifndef DEBUGMSG
#define spin_lock()\
    spinLock();\
    kprintf("%s call LOCK \n",__FUNCTION__);

#define spin_unlock(_lock_status)\
    spinUnLock(_lock_status);
    
#else
#define spin_lock() \
    spinLock();

#define spin_unlock(lock_status) \
    spinUnLock(lock_status);
#endif

/* uart */
int     uart_putc(char ch);
void    uart_puts(char *s);

/* printf */
int     kprintf(const char* s, ...);
void    panic(char *s);

/* mem */
void *  memset(void *ptr, int value, size_t num);
char *  memcpy(void* dest,const void* src, size_t num);

/* lock */
reg_t   spinLock();
void    spinUnLock(reg_t);
void    enableINT();
void    disableINT();

/* memory management */
void    *page_alloc(int npages);
void    page_free(void *p);
void    *malloc(uint32_t size);
void    free(void *);

/* task*/
void    readyQ_init();
void    InitTCBList(void);
taskCB_t * task_create(const char *name,
                  void (*taskFunc)(void *parameter),
                  void       *parameter,
                  uint32_t    stack_size,
                  uint16_t    priority,
                  uint32_t    ticks);
void    task_startup(taskCB_t * ptcb);
err_t   task_resume(taskCB_t *ptcb);
err_t   task_suspend(taskCB_t * ptcb);
err_t   task_yield(void);
taskCB_t *getCurrentTask();
void    taskDelay(uint32_t);
err_t   idleTask_init();


/* sched */
void    sched_init();
void    schedule();

/* trap */
void    trap_init();
reg_t   trap_handler(reg_t epc, reg_t cause);

/* timer */
void    timer_load(int interval);
void    timer_init();
void    timer_handler();
uint64_t getTicks();

/*softtimer*/
void    softTimer_init();
err_t   createTimer(uint8_t timerType,  
                  uint32_t timerCount,
                  uint32_t timerReload,
                  void(*callback)(void *parameter),
                  void *parameter
                  );
err_t   startTimer(uint16_t timerID);
err_t   stopTimer(uint16_t timerID);
err_t   delTimer(uint16_t timerID);
uint32_t    getCurTimerCnt(uint16_t timerID);
err_t   setCurTimerCnt(uint16_t timerID,
                     uint32_t timerCount,
                     uint32_t timerReload);
void    timerDispose(void);


/*IPC sem 09*/
err_t   createSem(uint16_t initCnt,uint16_t maxCnt,uint8_t sortType);
void    delSem(uint16_t semID);
/*
timeout = 0, try, -1: for ever
*/
err_t   sem_take(uint16_t semID, int timeout);
err_t   sem_trytake(uint16_t semID);
err_t   sem_release(uint16_t semID);

/* IPC mbox 09*/
err_t   createMbox(uint8_t sortType);
void    delMbox(uint16_t mboxID);
err_t   postMbox(uint16_t mboxID, void *pmail);
void *  acceptMail(uint16_t mboxID, err_t *perr);
void *  waitMail(uint16_t mboxID, uint32_t timeout, err_t *perr);



/* user*/
void    loadTasks(void);

/* lib */
/* queue */
void    TaskToWait(list_t *plist, uint8_t sortType, taskCB_t *ptcb);
void    WaitTaskToRdy(list_t *plist);
err_t   AllWaitTaskToRdy(list_t *plist);

#endif /* __OS_H__ */
