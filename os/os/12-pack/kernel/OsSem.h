#ifndef _SEM_H
#define _SEM_H

#include "types.h"

/*---------------------------- Resource status -------------------------------*/
#define   MUTEX_FREE        0           /*!< Mutex is free                    */
#define   MUTEX_OCCUPY      1           /*!< Mutex is occupy                  */
#define   WAITING_MUTEX     0x80

typedef struct sem
{
    list_t   semTCBList;              /*!< waitting the Sem.              */
    uint8_t  queueType;
    uint8_t  value; 
} sem_t;


/*---------------------------- Function declare ------------------------------*/
extern void   RemoveMutexList(P_OSTCB ptcb);

#endif    /* _SEM_H      */
