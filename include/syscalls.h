#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*! 
 * \brief Main syscall handler.
 * 
 * All syscalls are received by syscall_handler. 
 * getPID and getCPUTIME are handled autonomously,
 * while all other syscalls are delegated to specific 
 * static functions.
 */
void syscall_handler(void);

#endif