#ifndef _HANDLER_H_
#define _HANDLER_H_

time_t kernel_enter_tm;

void syscallHandler(void);
void interruptHandler(void);
void trapHandler(void);
void tlbExceptionHandler(void);

#endif