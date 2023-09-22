#ifndef _EVENT_H
#define _EVENT_H

#include "types.h"

/**
 * @struct  EventCtrBlk  event.h	  	
 * @brief   Event control block
 * @details This struct is use to manage event,
 *          e.g. semaphore,mailbox,queue.	
 */
typedef struct SemCB
{
    list_t        node;                    /*!< Task waitting list.              */
    uint16_t      id;                       /*!< ECB id                           */
    uint8_t       sortType;                 /*!< 0:FIFO 1: Preemptive by prio     */
    uint16_t      semCounter;               /*!< Counter of semaphore.            */
    uint16_t      initialCounter;          /*!< Initial counter of semaphore.    */
} SemCB_t;


#endif  