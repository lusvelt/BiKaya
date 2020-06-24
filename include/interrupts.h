#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "types.h"

void interrupts_handler(void);
int *interrupts_get_dev_key(devreg_t *, bool);

#endif