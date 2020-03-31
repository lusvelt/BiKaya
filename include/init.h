#ifndef _INIT_H_
#define _INIT_H_

#include "memory.h"
#include "system.h"

#define INIT_NEW_AREA(addr, handler)         \
    {                                        \
        state_t *area = (state_t *)addr;     \
        PC_SET(area, handler);               \
        SP_SET(area, RAM_TOP);               \
        uint32_t status = STATUS_GET(area);  \
        status = STATUS_DISABLE_INT(status); \
        status = SET_KERNEL_MODE(status);    \
        STATUS_SET(area, status);            \
        SET_VM_OFF(area);                    \
    }

void init(void);

#endif