#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "types.h"

void syscalls_handler(void);
int terminate_process(pid_t pid);

#endif