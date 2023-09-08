#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct Queue
{
    list_t   qTCBList;              /*!< waitting the Queu.              */
    uint8_t queueType;
    void    **qStart;                   /*!<                                  */
    U8      id;                         /*!<                                  */
    U16     head;                       /*!< The header of queue              */
    U16     tail;                       /*!< The end of queue                 */
    U16     qMaxSize;                   /*!< The max size of queue            */
    U16     qSize;                      /*!< Current size of queue            */
} queue_t;


#endif
