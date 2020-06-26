#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "types.h"

void interrupt_handler(void);
int *interrupt_get_dev_key(devreg_t *, bool);

#endif