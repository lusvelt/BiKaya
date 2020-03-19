#ifndef _INIT_H_
#define _INIT_H_

#include "memset.h"
#include "system.h"

#define INIT_NEW_AREA(addr, handler)                             \
    {                                                            \
        state_t *area = (state_t *)addr;                         \
        area->pc = (memaddr)handler;                             \
        area->sp = RAM_TOP;                                      \
        area->cpsr = STATUS_DISABLE_INT(STATUS_SYS_MODE);        \
        area->CP15_Control = CP15_DISABLE_VM(CP15_CONTROL_NULL); \
    }

void init(void);

#endif