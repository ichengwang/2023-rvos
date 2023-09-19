#include "os.h"

int spin_lock()
{
	w_mstatus(r_mstatus() & ~MSTATUS_MIE); //直接關閉中斷開關
	return 0;
}

int spin_unlock()
{
	w_mstatus(r_mstatus() | MSTATUS_MIE); //直接打開中斷開關
	return 0;
}
