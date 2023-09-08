#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int  uint32_t;
typedef unsigned long long uint64_t;
typedef char sint8_t;
typedef short sint16_t;
typedef int sint32_t;
typedef long long sint64_t;
/*
 * RISCV32: register is 32bits width
 */ 
typedef uint32_t reg_t;
typedef uint16_t OS_TID;
typedef sint8_t err_t;

#define NULL (void *)0
#define TRUE 1
#define FALSE 0
#define OK 0
#define ERROR -1
#define TIMEOUT -2

/*---------------------------- Error Codes   ---------------------------------*/
#define E_CREATE_FAIL         -1
#define E_OK                  0
#define E_TIMEOUT             1
#define E_SEM_FULL            2
#define E_MBOX_FULL           3
#define E_SEM_EMPTY           4
#define E_MBOX_EMPTY          5


/* Queue type */
#define FIFO  0
#define PRIO  1

/*---------------------------- Wait type  --------------------------*/
#define WAIT_NONE          0         /*!< Wait for all flags.              */
#define WAIT_FOREVER       1         /*!< Wait for any one of flags.       */
#define WAIT_TIME          2         /*!< Wat for one flag.               */	


typedef struct list {
    struct list *prev;
    struct list *next;
} list_t;

#endif
