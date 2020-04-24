#ifndef _UTILS_H_
#define _UTILS_H_

#include "terminal.h"
#include "types.h"
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