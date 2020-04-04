#ifndef _UTILS_H_
#define _UTILS_H_

#include "terminal.h"
#include "types.h"
#define EXIT(msg, ...)               \
    {                                \
        println(msg, ##__VA_ARGS__); \
        PANIC();                     \
    }

err_t createProcess(pcb_handler_t handler, uint8_t priority);

#endif