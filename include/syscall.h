#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "types.h"

void syscalls_handler(void);
void terminate_process(pid_t pid);

#endif