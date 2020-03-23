#include "types.h"
#include "uarm/arch.h"
#include "uarm/libuarm.h"

#define STATUS_GET(state) ((state)->cpsr)
#define PC_GET(state) ((state)->pc)
#define SP_GET(state) ((state)->sp)

#define STATUS_SET(state, val) ((state)->cpsr = (val))
#define PC_SET(state, val) ((state)->pc = (memaddr)(val))
#define SP_SET(state, val) ((state)->sp = (val))

#define SET_KERNEL_MODE(status) ((status) | STATUS_SYS_MODE)
#define SET_USER_MODE(status) ((status) | STATUS_USER_MODE)

#define SET_VM_OFF(state)                                 \
    {                                                     \
        uint32_t control = (state)->CP15_Control;         \
        (state)->CP15_Control = CP15_DISABLE_VM(control); \
    }

#define SET_VM_ON(state)                                 \
    {                                                    \
        uint32_t control = (state)->CP15_Control;        \
        (state)->CP15_Control = CP15_ENABLE_VM(control); \
    }
