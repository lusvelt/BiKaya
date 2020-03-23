#ifndef _TYPES_H_
#define _TYPES_H_
#ifdef TARGET_UMPS
#include <umps/types.h>
#endif
#ifdef TARGET_UARM
#define UARM_MACHINE_COMPILING
#include <uarm/uARMtypes.h>
#endif
#include <stdint.h>

#include "listx.h"

typedef unsigned int memaddr;

// Process Control Block (PCB) data structure
typedef struct pcb_t {
    /*process queue fields */
    struct list_head p_next;

    /*process tree fields */
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
} pcb_t;

typedef void (*pcb_handler_t)(void);

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
    OK = 0,
    ERR_NO_PROC
} err_t;

#endif
