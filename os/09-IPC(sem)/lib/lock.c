#include "os.h"

reg_t spinLock()
{
	reg_t status;
	status = r_mstatus();
	w_mstatus(status & ~MSTATUS_MIE); //直接關閉中斷開關
	return status;
}

void spinUnLock(reg_t status)
{
	w_mstatus(status);
	//kprintf("%s call unlock set=%x \n", status, __FUNCTION__);
}

void enableINT() 
{
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void disableINT()
{
	w_mstatus(r_mstatus() & ~MSTATUS_MIE);
}