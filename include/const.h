#ifndef _CONST_H_
#define _CONST_H_

#include "arch.h"

// #define DEBUG

/**************************************************************************** 
 *
 * This header file contains the global constant & macro definitions.
 * 
 ****************************************************************************/

/* Maxi number of overall (eg, system, daemons, user) concurrent processes */
#define MAXPROC 20

#define UPROCMAX 3 /* number of usermode processes (not including master proc and system daemons) */

#define NULL ((void *)0)
#define HIDDEN static
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define EOS '\0'

#define DEFAULT_PRIORITY 1
#define IDLE_PRIORITY (DEFAULT_PRIORITY - 1)

#define DEV_PER_INT 8 /* Maximum number of devices per interrupt line */

#define CMD_ACK 1

#define ST_READY 1
#define ST_BUSY 3

#define CR 0x0a /* carriage return as returned by the terminal */

/* System constants */
#define TIME_SLICE_MS 3
#define TIME_SCALE *((uint32_t *)BUS_REG_TIME_SCALE)
#define TIME_SLICE (TIME_SLICE_MS * 1000 * (TIME_SCALE))
#define TIMER_ACK (0xFFFFFFFF)

/* syscall types */
#define GETCPUTIME (1)
#define CREATEPROCESS (2)
#define TERMINATEPROCESS (3)
#define VERHOGEN (4)
#define PASSEREN (5)
#define WAITIO (6)
#define SPECPASSUP (7)
#define GETPID (8)

/* spec pass up types */
#define SPECPASSUP_SYSBK_TYPE (0)
#define SPECPASSUP_TLB_TYPE (1)
#define SPECPASSUP_TRAP_TYPE (2)

#endif
