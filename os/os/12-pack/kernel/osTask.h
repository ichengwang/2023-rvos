#ifndef __OSTASK_H__
#define __OSTASK_H__

#include "types.h"

/* task management */
typedef struct context {
	/* ignore x0 */
	reg_t ra;
	reg_t sp;
	reg_t gp;
	reg_t tp;
	reg_t t0;
	reg_t t1;
	reg_t t2;
	reg_t s0;
	reg_t s1;
	reg_t a0;
	reg_t a1;
	reg_t a2;
	reg_t a3;
	reg_t a4;
	reg_t a5;
	reg_t a6;
	reg_t a7;
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
	reg_t t3;
	reg_t t4;
	reg_t t5;
	reg_t t6;
	// upon is trap frame

	// save the pc to run in next schedule cycle
	reg_t pc; // offset: 31 *4 = 124
} ctx_t;


#define TASK_INIT                  0x00                /**< Initialized status */
#define TASK_READY                 0x01                /**< Ready status */
#define TASK_SUSPEND               0x02                /**< Suspend status */
#define TASK_RUNNING               0x03                /**< Running status */


/**
 * Task structure
 */
typedef struct taskCB
{
	list_t      node;
    char        name[10];
	uint32_t 	taskID;

    /* stack point and entry */
    void       *sp;                                     /**< stack point */
    void       *entry;                                  /**< entry */
    void       *parameter;                              /**< parameter */
    void       *stack_addr;                             /**< stack address */
    uint32_t   stack_size;                             /**< stack size */

    ctx_t      ctx;    

    uint8_t  state;                                   /**< task status */
	sint8_t	 error;

    /* priority */
    uint8_t  current_priority;                       /**< current priority */
    uint8_t  init_priority;                          /**< initialized priority */

    uint32_t  init_tick;                              /**< task's initialized tick */
    uint32_t  remaining_tick;                         /**< remaining tick */

    uint32_t   tmrID;                       /**< built-in thread timer */

} taskCB_t;

#endif /* __OSTASK_H__ */
