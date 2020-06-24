#ifndef _MACRO_H_
#define _MACRO_H_

#include "arch.h"
#include "memory.h"
#include "terminal.h"

#define INIT_NEW_AREA(area, handler)                                 \
    {                                                                \
        memset(area, 0, sizeof(state_t));                            \
        PC(*area) = (uint32_t)handler;                               \
        SP(*area) = RAM_TOP;                                         \
        STATUS(*area) = KERNEL_MODE(*area) | ALL_INT_DISABLE(*area); \
        VM(*area) &= VM_OFF;                                         \
    }

#define EXIT(msg, ...)               \
    {                                \
        println(msg, ##__VA_ARGS__); \
        PANIC();                     \
    }

#define HALT(msg, ...)               \
    {                                \
        println(msg, ##__VA_ARGS__); \
        HALT();                      \
    }

#endif