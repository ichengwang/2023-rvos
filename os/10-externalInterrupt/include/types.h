#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int  uint32_t;
typedef unsigned long long uint64_t;

/*
 * RISCV32: register is 32bits width
 */ 
typedef uint32_t reg_t;
typedef uint8_t state_t;
typedef char err_t;

#define NULL (void *)0
#define TRUE 1
#define FALSE 0
#define OK 0
#define ERROR -1

/*---------------------------- Error Codes   ---------------------------------*/
#define E_CREATE_FAIL         -1
#define E_OK                    0
#define E_EMPTY                 1  //09-sem
#define E_FULL                  2  //09-sem
#define E_TIMEOUT               3  //waiting time out 09-sem          0

typedef struct list {
    struct list *prev;
    struct list *next;
} list_t;


/* lock struct*/
typedef struct spinlock {
	volatile uint32_t locked; 		//判斷是否有被佔有(佔有:1，未佔有:0)
} spinlock_t;

/****
 * 08
*/
#define DEBUGMSG

#ifndef DEBUGMSG
    #define DEBUG(f_, ...)
#else
    #define DEBUG(f_, ...) kprintf((f_), ##__VA_ARGS__)
#endif


//08 
#define MAP_SIZE sizeof(uint32_t)


/* 09  Queue Type*/
#define FIFO 0
#define PRIO 1
#endif /* __TYPES_H__ */