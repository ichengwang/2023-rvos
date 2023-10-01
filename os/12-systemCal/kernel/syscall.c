#include "os.h"

extern void*(*sysFunc[])(void *p);

//ISR do_syscall here
int sys_gethid(unsigned int *ptr_hid)
{
	DEBUG_ISR("--> sys_gethid, arg0 = 0x%x\n", ptr_hid);
	if (ptr_hid == NULL) {
		DEBUG_ISR("ptr_hid == NULL\n");
       		return -1;
	} else {
		DEBUG_ISR("ptr_hid != NULL\n");
		 *ptr_hid = r_mhartid();
 		 return 0;
	}
}

static void * _sys_gethid(void *p) {
    int rst = sys_gethid((unsigned int *)p);
    return (void *)rst;
}

static void *_sys_delay(void *p) {
    taskDelay((uint32_t)p);
    return NULL;
}

static void * _sem_create(void *p) {
    uint32_t *v = (uint32_t*)p;
    err_t i = createSem((uint16_t)v[0],(uint16_t)v[1],(uint8_t)v[2]);
    return (void*)i;
}

static void * _sem_take(void *p) {
    int *v=(int*)p;
    err_t i = sem_take((uint16_t)v[0], v[1]);
    return (void*)i;
}

static void * _sem_release(void *p) {
    err_t i = sem_release((uint16_t)p);
    return (void*)i;
}



void syscall_init() {
    for (int i=0; i<MAX_SYSCALL;i++)
        sysFunc[i]=NULL;
    
    //register all syscall here
    syscall_register(GETHID, _sys_gethid);
    syscall_register(DELAY, _sys_delay);
    syscall_register(SEM_TAKE, _sem_take);
    syscall_register(SEM_RELEASE, _sem_release);
    syscall_register(SEM_CREATE, _sem_create);
}