#include "syscall.h"

#include "asl.h"
#include "scheduler.h"
#include "system.h"
#include "terminal.h"

HIDDEN int devices[8][8];

syscall_ret_t syscall_create_process(state_t *state, int priority, void **cpid) {
    pcb_t *p = pcb_alloc();

    if (p == NULL)
        return SYSCALL_FAILURE;

    p->p_s = *state;

    p->original_priority = priority;
    p->priority = priority;

    debugln("Created process %p (pc = %p)", p, p->p_s.pc);
    pcb_t *current = getCurrent();
    pcb_insert_child(current, p);

    addToReadyQueue(p);

    if (cpid)
        *cpid = p;

    return SYSCALL_SUCCESS;
}

syscall_ret_t syscall_terminate_process(pcb_t *pid) {
    if (pid == NULL)
        pid = getCurrent();
    if (pid == NULL)
        return SYSCALL_FAILURE;

    killProgeny(pid);
    return SYSCALL_SUCCESS;
}

syscall_ret_t syscall_verhogen(int *semaddr) {
    semd_t *semd = asl_semd(semaddr);
    if (semd == NULL || list_empty(&semd->s_procQ)) {
        (*semaddr)++;
    } else {
        pcb_t *blocked = asl_remove_blocked(semaddr);
        addToReadyQueue(blocked);
    }
    return SYSCALL_SUCCESS;
}

// Returns 1 if process has been blocked, 0 otherwise
bool syscall_passeren(int *semaddr, pcb_t *pid) {
    if (*semaddr) {
        (*semaddr)--;
        return 0;
    } else {
        removeHeadFromReadyQueue();
        asl_insert_blocked(semaddr, pid);
        return 1;
    }
}

#define SET_COMMAND(reg, subdev, command) (*((uint32_t *)(reg) + 1 + (2 * (1 - subdev))) = (command))

void syscall_waitio(uint32_t command, devreg_t *reg, bool subdev) {
    pcb_t *current = getCurrent();
    int *semKey = syscall_get_device_sem_key(reg);
    SET_COMMAND(reg, subdev, command);
    *semKey = 0;
    removeHeadFromReadyQueue();
    if (asl_insert_blocked(semKey, current))
        EXIT("Too many semaphores allocated.");
}

/* This macro should only be used inside a function that returns syscall_ret_t */
#define REGISTER_SPU_HANDLER(p, field, old, new) \
    {                                            \
        if (p->field##_new != NULL) {            \
            syscall_terminate_process(p);        \
            return SYSCALL_FAILURE;              \
        }                                        \
        p->field##_old = old;                    \
        p->field##_new = new;                    \
    }

syscall_ret_t syscall_specpassup(spu_t type, state_t *old, state_t *new) {
    pcb_t *p = getCurrent();

    switch (type) {
        case SPU_SYSCALL_BRK:
            REGISTER_SPU_HANDLER(p, sysbk, old, new);
            break;
        case SPU_TLB:
            REGISTER_SPU_HANDLER(p, tlb, old, new);
            break;
        case SPU_TRAP:
            REGISTER_SPU_HANDLER(p, trap, old, new);
            break;
    }
    return SYSCALL_SUCCESS;
}

syscall_ret_t syscall_getpid(pcb_t *p, uint32_t *pid, uint32_t *ppid) {
    if (pid)
        *pid = (uint32_t)p;
    if (ppid)
        *ppid = (uint32_t)p->p_parent;
    return SYSCALL_SUCCESS;
}

int *syscall_get_device_sem_key(devreg_t *reg) {
    int line, dev;
    int tmp = ((uint32_t)reg - DEV_REG_START) / DEV_REG_SIZE;
    dev = tmp % N_DEV_PER_IL;
    line = (tmp - dev) / N_DEV_PER_IL + DEV_IL_START;
    return &devices[line][dev];
}