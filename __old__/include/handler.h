#ifndef _HANDLER_H_
#define _HANDLER_H_

void handler_syscall(void);
void handler_interrupt(void);
void handler_trap(void);
void handler_tlb_exception(void);

#endif