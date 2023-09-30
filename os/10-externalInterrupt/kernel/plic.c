#include "os.h"
//vector table 

static void (*ISRTbl[MAX_EXTERNAL_INT])(void);


void isrRegister(int irq, void(*isr)(void)) {
    reg_t lock_status = spin_lock();
    ISRTbl[irq] = isr;
    spin_unlock(lock_status);
}

void externalInterrupt() {
    int irq = plic_claim();
    if (ISRTbl[irq]) 
        ISRTbl[irq]();
    else 
        kprintf("unexpected interrupt irq = %d\n", irq);

    if (irq)
        plic_complete(irq);
}


/* hardware initial*/
void plic_init(void)
{
    //initial ISR Table
    for (int i=0;i<MAX_EXTERNAL_INT;i++)
        ISRTbl[i]=NULL;

	reg_t hart = r_mhartid();
	/* 
	 * 1. Set priority for UART0.
	 */
	*(uint32_t*)PLIC_PRIORITY(UART0_IRQ) = 1;
 
	/*
	 * 2. Enable UART0
	 */
	*(uint32_t*)PLIC_MENABLE(hart)= (1 << UART0_IRQ);

	/* 
	 * 3. Set priority threshold for UART0.
	 */
	*(uint32_t*)PLIC_MTHRESHOLD(hart) = 0;

	/* enable machine-mode external interrupts. */
	w_mie(r_mie() | MIE_MEIE);
}


int plic_claim(void)
{
	int hart = r_mhartid();
	int irq = *(uint32_t*)PLIC_MCLAIM(hart);
	return irq;
}


void plic_complete(int irq)
{
	int hart = r_mhartid();
	*(uint32_t*)PLIC_MCOMPLETE(hart) = irq;
}
