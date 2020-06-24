#ifndef _MACRO_H_
#define _MACRO_H_

#include "arch.h"
#include "memory.h"
#include "terminal.h"

#define INIT_NEW_AREA(area, handler)                                 \
    {                                                                \
        state_t state;                                               \
        memcpy(&state, (state_t *)area, sizeof(state_t));            \
        PC(state) = (uint32_t)handler;                               \
        SP(state) = RAM_TOP;                                         \
        STATUS(state) = KERNEL_MODE(state) | ALL_INT_DISABLE(state); \
        VM(state) &= VM_OFF;                                         \
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