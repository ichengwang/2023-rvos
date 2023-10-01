#include "os.h"

extern void trap_vector(void);

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	w_mtvec((reg_t)trap_vector);
}

reg_t trap_handler(reg_t epc, reg_t cause)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & 0xfff;


	if (cause_code == 11) {
		return_pc=epc;
	}

	
	if (cause & 0x80000000) {
		/* Asynchronous trap - interrupt */
		switch (cause_code) {
		case 3:
			//DEBUG_ISR("software interruption!\n");
			/*
			 * acknowledge the software interrupt by clearing
    			 * the MSIP bit in mip.
			 */
			int id = r_mhartid();
    		*(uint32_t*)CLINT_MSIP(id) = 0;
			//DEBUG_ISR("schedule ISR\n");
			schedule();			
			break;
		case 7:
			//DEBUG_ISR("timer ISR\n");
			timer_handler();
			break;
		case 11:
			//DEBUG_ISR("external ISR\n");
			externalInterrupt();
			break;
		default:
			DEBUG_ISR("unknown async exception!\n");
			break;
		}
	} else {
		/* Synchronous trap - exception */
		DEBUG_ISR("Sync exceptions!, code = %d\n", cause_code);
		panic("OOPS! What can I do!");
		//return_pc += 4;
	}

	return return_pc;
}


void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	
	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	int a = *(int *)0x00000000;

	a = 100;
	DEBUG("Yeah! I'm return back from trap!\n");
}

