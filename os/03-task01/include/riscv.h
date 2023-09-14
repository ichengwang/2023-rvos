#ifndef __RISCV_H__
#define __RISCV_H__

#include "types.h"


static inline void w_mscratch(reg_t x)
{
	asm volatile("csrw mscratch, %0" : : "r" (x));
}

#endif /* __RISCV_H__ */
