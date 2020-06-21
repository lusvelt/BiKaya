#ifndef _TYPES_H_
#define _TYPES_H_
#ifdef TARGET_UMPS
#include "umps/types.h"
#endif
#ifdef TARGET_UARM
#define UARM_MACHINE_COMPILING
#include "uarm/uARMtypes.h"
#endif

#include <stdint.h>

#include "listx.h"

typedef uint64_t time_t;

// Process Control Block (PCB) data structure
typedef struct pcb_t {
    /* process queue fields */
    struct list_head p_next;

    /* process tree fields */
    struct pcb_t *p_parent;
    struct list_head p_child, p_sib;

    /* processor state, etc */
    state_t p_s;

    /* dynamic process priority */
    int priority;
    /* initial process priority */
    int original_priority;

    /* key of the semaphore on which the process is eventually blocked */
    int *p_semkey;

    /* time tracking fields */
    time_t start_tm, user_tm, kernel_tm;

    /* handler fields */
    state_t exc_new_areas[3];
    state_t exc_old_areas[3];

} pcb_t;

typedef void (*pcb_code_t)(void);
typedef unsigned int pid_t;

// Semaphore Descriptor (SEMD) data structure
typedef struct semd_t {
    struct list_head s_next;

    // Semaphore key
    int *s_key;

    // Queue of PCBs blocked on the semaphore
    struct list_head s_procQ;
} semd_t;

typedef uint8_t bool;

typedef enum {
    SYSCALL_SUCCESS = 0,
    SYSCALL_FAILURE = -1
} syscall_ret_t;

typedef enum {
    GETCPUTIME = 1,
    CREATEPROCESS,
    TERMINATEPROCESS,
    VERHOGEN,
    PASSEREN,
    WAITIO,
    SPECPASSUP,
    GETPID,
} syscall_t;

typedef enum {
    SPU_SYSCALL_BRK = 0,
    SPU_TLB = 1,
    SPU_TRAP = 2
} spu_t;

#endif
