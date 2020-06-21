#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "pcb.h"

void syscall_handler(void);

syscall_ret_t syscall_create_process(state_t *state, int priority, void **cpid);
syscall_ret_t syscall_terminate_process(pcb_t *pid);
syscall_ret_t syscall_verhogen(int *semaddr);
bool syscall_passeren(int *semaddr, pcb_t *pid);
void syscall_waitio(uint32_t command, devreg_t *reg, bool subdev);
syscall_ret_t syscall_specpassup(spu_t type, state_t *old, state_t *new);
syscall_ret_t syscall_getpid(pcb_t *p, uint32_t *pid, uint32_t *ppid);

int *syscall_get_device_sem_key(devreg_t *reg);

#endif