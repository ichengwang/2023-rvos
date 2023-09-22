#include "os.h"

reg_t spin_lock()
{
	reg_t status;
	status = r_mstatus();
	w_mstatus(status & ~MSTATUS_MIE); //直接關閉中斷開關
	return status;
}

void spin_unlock(reg_t status)
{
	w_mstatus(status); 
}
