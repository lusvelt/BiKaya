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

#define CAUSE_GET(state) ((state)->CP15_Cause)
#define CAUSE_IS_SYSCALL(cause) (CAUSE_EXCCODE_GET(cause) == EXC_SYSCALL)
#define CAUSE_GET_LINE(cause) (((cause) >> 24) & 0xFF)

#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define REG_GET(state, reg) (*((uint32_t *)((state) + (reg))))

#define SET_TIMER(time) setTIMER(time)
#define INT_IS_PENDING(cause, line) CAUSE_IP_GET((cause), (line))

#define SYSCALL_RETURN(state, val) ((state)->a1 = (val))
