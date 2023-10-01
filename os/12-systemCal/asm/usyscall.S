#include "syscallTable.h"
.text
.globl gethid
.align 4
gethid:
    li a7, GETHID
    ecall
    ret

.globl delay
.align 4
delay:
    li a7, DELAY
    ecall
    ret

.globl u_sem_take
.align 4
u_sem_take:
    li a7, SEM_TAKE
    ecall
    ret

.globl u_sem_release
.align 4
u_sem_release:
    li a7, SEM_RELEASE
    ecall
    ret

.globl u_sem_create
.align 4
u_sem_create:
    li a7, SEM_CREATE
    ecall
    ret

.end