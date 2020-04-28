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

typedef unsigned int memaddr;
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

typedef struct semdev {
    semd_t disk[DEV_PER_INT];
    semd_t tape[DEV_PER_INT];
    semd_t network[DEV_PER_INT];
    semd_t printer[DEV_PER_INT];
    semd_t terminalR[DEV_PER_INT];
    semd_t terminalT[DEV_PER_INT];
} semdev;

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

#endif
