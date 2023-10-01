#ifndef _IPC_H
#define _IPC_H

#include "types.h"

typedef struct SemCB
{
    list_t        node;                    /*!< Task waitting list.              */
    uint16_t      id;                       /*!< ECB id                           */
    uint8_t       sortType;                 /*!< 0:FIFO 1: Preemptive by prio     */
    uint16_t      semCounter;               /*!< Counter of semaphore.            */
    uint16_t      initialCounter;          /*!< Initial counter of semaphore.    */
} SemCB_t;


typedef struct MboxCB
{
    list_t        node;                    /*!< Task waitting list.              */
    uint16_t      id;                       /*!< ECB id                           */
    uint8_t       sortType;                 /*!< 0:FIFO 1: Preemptive by prio     */
    uint8_t       mailCount;                // if mailCount==1 has a mail
    void *        mailPtr;
} MboxCB_t;


#endif  