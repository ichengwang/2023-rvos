#ifndef _TIMER_H
#define _TIMER_H

#include "types.h"

#define   TMR_TYPE_ONE_SHOT     0         /*!< Timer counter type: One-shot     */
#define   TMR_TYPE_PERIODIC     1         /*!< Timer counter type: Periodic     */

#define   TMR_STATE_RUNNING     0       /*!< Timer State: Running             */
#define   TMR_STATE_STOPPED     1       /*!< Timer State: Stopped             */

typedef struct tmrCtrl                  /* Timer Control Block Define.        */
{
    uint32_t              tmrID;             /*!< Timer ID.                        */
    uint8_t               tmrType;           /*!< Timer Type.                      */
    uint8_t               tmrState;          /*!< Timer State.                     */
    uint8_t               tmrCnt;            /*!< Timer Counter.                   */
    uint32_t              tmrReload;         /*!< Timer Reload Counter Value.      */	
    void (*tmrCallBack)(void *parameter);     /*!< Call-back Function When Timer overrun. */	
    void *parameter;
    struct tmrCtrl*  tmrNext;       /*!< Point to Next Timer Control Block.   */
    struct tmrCtrl*  tmrPrev;       /*!< Point to Previous Timer Control Block*/

} timer_t;

#endif
