#include "uarm/arch.h"
#include "uarm/libuarm.h"

#define STATUS(state) ((state).cpsr)
#define PC(state) ((state).pc)
#define SP(state) ((state).sp)
#define VM(state) ((state).CP15_Control)
#define VM_ON (CP15_VM_ON)
#define VM_OFF ~(VM_ON)

#define KERNEL_MODE(state) (STATUS(state) | STATUS_SYS_MODE)
#define USER_MODE(state) (STATUS(state) | STATUS_USER_MODE)

#define CAUSE_SYSCALL EXC_SYSCALL
#define CAUSE_BREAKPOINT EXC_BREAKPOINT
#define CAUSE(state) ((state).CP15_Cause)
#define CAUSE_EXC(state) (CAUSE_EXCCODE_GET(CAUSE(state)))

#define SYSARG0(state) ((state).a1)
#define SYSARG1(state) ((state).a2)
#define SYSARG2(state) ((state).a3)
#define SYSARG3(state) ((state).a4)
#define SYSRETURN(state) ((state).a1)

#define ALL_INT_ENABLE(state) (STATUS_ALL_INT_ENABLE(STATUS(state)))
#define ALL_INT_DISABLE(state) (STATUS_ALL_INT_DISABLE(STATUS(state)))

#define INT_IS_PENDING(cause, line) CAUSE_IP_GET((cause), (line))
#define TERM_0_ADDR (0x00000240)
// no need for lower bound as device pointers are from DEV_REG_ADDR
#define IS_TERMINAL(device) ((uint32_t)device >= TERM_0_ADDR)
