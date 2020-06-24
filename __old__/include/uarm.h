#include "types.h"
#include "uarm/arch.h"
#include "uarm/libuarm.h"

// #define STATUS_GET(state) ((state)->cpsr)
// #define PC_GET(state) ((state)->pc)
// #define SP_GET(state) ((state)->sp)

// #define STATUS_SET(state, val) ((state)->cpsr = (val))
// #define PC_SET(state, val) ((state)->pc = (uint32_t)(val))
// #define SP_SET(state, val) ((state)->sp = (val))

#define STATUS_REG(state) ((state).cpsr)
#define PC(state) ((state).pc)
#define SP(state) ((state).sp)

#define VM(state) ((state).CP15_Control)
#define VM_ON (CP15_VM_ON)
#define VM_OFF ~(VM_ON)

// #define SET_KERNEL_MODE(status) ((status) | STATUS_SYS_MODE)
// #define SET_USER_MODE(status) ((status) | STATUS_USER_MODE)
#define KERNEL_MODE STATUS_SYS_MODE
#define USER_MODE STATUS_USER_MODE

// #define SET_VM_OFF(state)                                 \
//     {                                                     \
//         uint32_t control = (state)->CP15_Control;         \
//         (state)->CP15_Control = CP15_DISABLE_VM(control); \
//     }

// #define SET_VM_ON(state)                                 \
//     {                                                    \
//         uint32_t control = (state)->CP15_Control;        \
//         (state)->CP15_Control = CP15_ENABLE_VM(control); \
//     }

#define CAUSE_GET(state) ((state)->CP15_Cause)
#define CAUSE_IS_SYSCALL(cause) (CAUSE_EXCCODE_GET(cause) == EXC_SYSCALL)
#define CAUSE_GET_LINE(cause) (((cause) >> 24) & 0xFF)

#define CAUSE(state) (CAUSE_EXCCODE_GET((state).CP15_Cause))
#define CAUSE_SYSCALL EXC_SYSCALL

// #define A0 a1
// #define A1 a2
// #define A2 a3
// #define A3 a4
// #define REG_GET(state, reg) ((state)->reg)

#define SYSARG0(state) ((state).a1)
#define SYSARG1(state) ((state).a2)
#define SYSARG2(state) ((state).a3)
#define SYSARG3(state) ((state).a4)

#define SET_TIMER(time) setTIMER(time)
#define INT_IS_PENDING(cause, line) CAUSE_IP_GET((cause), (line))

#define SYSCALL_RETURN(state, val) ((state)->a1 = (val))
#define SYSRETURN(state) ((state).a1)