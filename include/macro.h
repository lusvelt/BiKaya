#ifndef _MACRO_H_
#define _MACRO_H_

#define INIT_NEW_AREA(addr, handler)             \
    {                                            \
        state_t *area = (state_t *)addr;         \
        PC_SET(area, handler);                   \
        SP_SET(area, RAM_TOP);                   \
        uint32_t status = STATUS_GET(area);      \
        status = STATUS_ALL_INT_DISABLE(status); \
        status = SET_KERNEL_MODE(status);        \
        STATUS_SET(area, status);                \
        SET_VM_OFF(area);                        \
    }

#endif