#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "pcb.h"

syscall_ret_t createProcess(state_t *state, int priority, void **cpid);
syscall_ret_t terminateProcess(pcb_t *pid);
syscall_ret_t verhogen(int *semaddr);
syscall_ret_t passeren(int *semaddr, pcb_t *pid);
syscall_ret_t specPassUp(spu_t type, state_t *old, state_t *new);
syscall_ret_t getPid(pcb_t *p, void **pid, void **ppid);

#endif