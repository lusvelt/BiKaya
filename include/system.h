#ifdef TARGET_UMPS
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

#define pc pc_epc
#define sp reg_sp
#define RAM_SIZE ((int)(*((int *)0x10000004)))
#define RAM_TOP (RAM_BASE + RAM_SIZE)
#define cpsr status

#endif
#ifdef TARGET_UARM
#include "uarm/arch.h"
#include "uarm/libuarm.h"
#include "uarm/uARMtypes.h"
#endif

#include "const.h"
#include "types_bikaya.h"
