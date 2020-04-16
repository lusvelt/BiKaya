#include "types.h"
#include "umps/arch.h"
#include "umps/cp0.h"
#include "umps/libumps.h"
#include "umps/regdef.h"

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

#define STATUS_GET(state) ((state)->status)
#define PC_GET(state) ((state)->pc_epc)
#define SP_GET(state) ((state)->reg_sp)

#define STATUS_SET(state, val) ((state)->status = (val))
#define PC_SET(state, val) ((state)->pc_epc = (memaddr)(val))
#define SP_SET(state, val) ((state)->reg_sp = (val))

// We set IEp instead of IEc due to shifting operations in LDST
#define STATUS_ID 0x00000004
// using INTERVAL timer
#define STATUS_TIMER_ID 0x00000400
// consistently with uARM, masks all ints except timer
#define STATUS_ID_MASK 0x0000FB00
#define STATUS_ALL_INT_DISABLE(status) ((status) & ~STATUS_ID)
#define STATUS_ALL_INT_ENABLE(status) ((status) | STATUS_ID)

#define INT_LOWEST 3    /* minimum interrupt number used by real devices */
#define DEV_USED_INTS 5 /* Number of ints reserved for devices: 3,4,5,6,7 */

// in order to be consistent with uARM, STATUS_ENABLE_INT enables all interrupts
// except timer, leaving timer UNCHANGED
#define STATUS_ENABLE_INT(status) ((status) | STATUS_ID_MASK | STATUS_ID)
// analogous to STATUS_ENABLE_INT
#define STATUS_DISABLE_INT(status) ((status) & ~STATUS_ID_MASK)

#define STATUS_ENABLE_TIMER(status) ((status) | STATUS_TIMER_ID | STATUS_ID)
#define STATUS_DISABLE_TIMER(status) ((status) & ~STATUS_TIMER_ID)

#define STATUS_KUC 0x00000002
#define SET_KERNEL_MODE(status) ((status) & ~STATUS_KUC)
#define SET_USER_MODE(status) ((status) | STATUS_KUC)

#define STATUS_VMC 0x01000000
#define SET_VM_OFF(state)                    \
    {                                        \
        uint32_t status = STATUS_GET(state); \
        status = status & ~STATUS_VMC;       \
        STATUS_SET(state, status);           \
    }

#define SET_VM_ON(state)                     \
    {                                        \
        uint32_t status = STATUS_GET(state); \
        status = status | STATUS_VMC;        \
        STATUS_SET(state, status);           \
    }

#define CAUSE_GET(state) ((state)->cause)
#define CAUSE_IS_SYSCALL(cause) (CAUSE_GET_EXCCODE(cause) == EXC_SYS)

#define REG_GET(state, reg) (*((uint32_t *)((state)->gpr + reg - 1)))

#define TIMER_LINE IL_TIMER
#define IS_TIMER_INT(cause) CAUSE_IP(TIMER_LINE) & (cause)
#define SET_TIMER(time) (*((uint32_t *)BUS_REG_TIMER) = (time))

/* Values for CP0 Cause.ExcCode */
#define EXC_INTERRUPT EXC_INT
#define EXC_TLBMOD EXC_MOD
#define EXC_TLBINVLOAD EXC_TLBL
#define EXC_TLBINVSTORE EXC_TLBS
#define EXC_ADDRINVLOAD EXC_ADEL
#define EXC_ADDRINVSTORE EXC_ADES
#define EXC_BUSINVFETCH EXC_IBE
#define EXC_BUSINVLDSTORE EXC_DBE
#define EXC_SYSCALL EXC_SYS
#define EXC_BREAKPOINT EXC_BP
#define EXC_RESERVEDINSTR EXC_RI
#define EXC_COPROCUNUSABLE EXC_CPU
#define EXC_ARITHOVERFLOW EXC_OV
#define EXC_BADPTE EXC_BDPT  /* uMPS-specific */
#define EXC_PTEMISS EXC_PTMS /* uMPS-specific */
