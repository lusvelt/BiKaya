#include "umps/arch.h"
#include "umps/libumps.h"
#include "umps/types.h"

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
#define PC_SET(state, val) ((state)->pc_epc = (val))
#define SP_SET(state, val) ((state)->reg_sp = (val))

#define STATUS_ID 0x00000001
#define STATUS_TIMER_ID 0x00000400
#define STATUS_ID_MASK 0x0000FF00
#define STATUS_ALL_INT_DISABLE(status) ((status) & ~STATUS_ID)
#define STATUS_ALL_INT_ENABLE(status) ((status) | STATUS_ID)

#define STATUS_ENABLE_INT(status) ((status) | (STATUS_ID_MASK & ~STATUS_TIMER_ID))
#define STATUS_DISABLE_INT(status) ((status) & ~(STATUS_ID_MASK & ~STATUS_TIMER_ID))

#define STATUS_ENABLE_TIMER(status) ((status) | STATUS_TIMER_ID)
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
