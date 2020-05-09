#ifndef _HANDLER_H_
#define _HANDLER_H_

void syscallHandler(void);
void interruptHandler(void);
void trapHandler(void);
void tlbExceptionHandler(void);

#endif