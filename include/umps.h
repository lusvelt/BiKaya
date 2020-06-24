#include "umps/arch.h"
#include "umps/cp0.h"
#include "umps/libumps.h"

#define EXCV_BASE 0x20000000
#define STATE_T_SIZE 0x8C
#define INT_OLDAREA EXCV_BASE
#define INT_NEWAREA (EXCV_BASE + STATE_T_SIZE)
#define TLB_OLDAREA (EXCV_BASE + (2 * STATE_T_SIZE))
#define TLB_NEWAREA (EXCV_BASE + (3 * STATE_T_SIZE))
#define PGMTRAP_OLDAREA (EXCV_BASE + (4 * STATE_T_SIZE))
#define PGMTRAP_NEWAREA (EXCV_BASE + (5 * STATE_T_SIZE))
#define SYSBK_OLDAREA (EXCV_BASE + (6 * STATE_T_SIZE))
#define SYSBK_NEWAREA (EXCV_BASE + (7 * STATE_T_SIZE))

#define RAM_SIZE ((int)(*((int *)0x10000004)))
#define RAM_TOP (RAM_BASE + RAM_SIZE)
#define FRAME_SIZE 4096

#define STATUS(state) ((state).status)
#define PC(state) ((state).pc_epc)
#define SP(state) ((state).reg_sp)
#define VM(state) ((state).status)
#define VM_ON STATUS_VMc
#define VM_OFF ~(VM_ON)

#define KERNEL_MODE(state) (STATUS(state) & ~STATUS_KUc)
#define USER_MODE(state) (STATUS(state) | STATUS_KUc)

#define CAUSE_SYSCALL EXC_SYS
#define CAUSE(state) ((state).cause)
#define CAUSE_EXC(state) (CAUSE_GET_EXCCODE(CAUSE(state)))

#define SYSARG0(state) ((state).reg_a0)
#define SYSARG1(state) ((state).reg_a1)
#define SYSARG2(state) ((state).reg_a2)
#define SYSARG3(state) ((state).reg_a3)
#define SYSRETURN(state) ((state).reg_v0)

// STATUS_IEp globally enables/disables all interrupts
// STATUS_IM_MASK enables/disables a specific set of interrupts
#define ALL_INT_DISABLE(state) (STATUS(state) & ~STATUS_IEp)
#define ALL_INT_ENABLE(state) (STATUS(state) | STATUS_IM_MASK)

#define INT_IS_PENDING(cause, line) ((cause)&CAUSE_IP(line))
#define TERM_0_ADDR (0x10000250)
// no need for lower bound as device pointers are from DEV_REG_ADDR
#define IS_TERMINAL(device) ((uint32_t)device >= TERM_0_ADDR)

#define getTODLO() (*((uint32_t *)BUS_TODLOW))
#define getTODHI() (*((uint32_t *)BUS_TODHI))
#define setTIMER(time) (*((uint32_t *)BUS_REG_TIMER) = (time))

// I don't think these are necessary
//#define INT_LOWEST 3    /* minimum interrupt number used by real devices */
//#define DEV_USED_INTS 5 /* Number of ints reserved for devices: 3,4,5,6,7 */
