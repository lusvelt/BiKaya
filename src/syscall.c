#include "syscall.h"

#include "asl.h"
#include "scheduler.h"
#include "system.h"
#include "terminal.h"

HIDDEN int devices[8][8];

syscall_ret_t createProcess(state_t *state, int priority, void **cpid) {
    pcb_t *p = allocPcb();

    if (p == NULL)
        return SYSCALL_FAILURE;

    p->p_s = *state;

    p->original_priority = priority;
    p->priority = priority;

    pcb_t *current = getCurrent();
    insertChild(current, p);

    addToReadyQueue(p);

    if (cpid)
        *cpid = p;

    return SYSCALL_SUCCESS;
}

syscall_ret_t terminateProcess(pcb_t *pid) {
    if (pid == NULL)
        pid = getCurrent();
    if (pid == NULL)
        return SYSCALL_FAILURE;
    killProgeny(pid);
    return SYSCALL_SUCCESS;
}

syscall_ret_t verhogen(int *semaddr) {
    semd_t *semd = getSemd(semaddr);
    println("the sem descriptor is in fact: %p", semd);
    println("list of processes on this sem is %d", list_empty(&semd->s_procQ));
    if (semd == NULL || list_empty(&semd->s_procQ)) {
        println("increasing *semaddr, now at %d", *semaddr);
        (*semaddr)++;
    } else {
        println("since list was NOT empty, adding blocked process");
        addToReadyQueue(removeBlocked(semaddr));
    }
    return SYSCALL_SUCCESS;
}

// Returns 1 if process has been blocked, 0 otherwise
bool passeren(int *semaddr, pcb_t *pid) {
    if (*semaddr) {
        println("decrementing *semaddr, which now is %d", *semaddr);
        (*semaddr)--;
        println("and it's now become %d", *semaddr);
        return 0;
    } else {
        println("not going through this code");
        removeHeadFromReadyQueue();
        insertBlocked(semaddr, pid);
        println("or this");
        return 1;
    }
}

#define SET_COMMAND(reg, subdev, command) (*((uint32_t *)(reg) + 1 + (2 * (1 - subdev))) = (command))

void waitIo(uint32_t command, devreg_t *reg, bool subdev) {
    println("entered waitIO function");
    pcb_t *current = getCurrent();
    int *semKey = getDeviceSemKey(reg);
    SET_COMMAND(reg, subdev, command);
    *semKey = 0;
    removeHeadFromReadyQueue();
    if (insertBlocked(semKey, current))
        EXIT("Too many semaphores allocated.");
}

/* This macro should only be used inside a function that returns syscall_ret_t */
#define REGISTER_SPU_HANDLER(p, field, old, new) \
    {                                            \
        if (p->field##_new != NULL) {            \
            terminateProcess(p);                 \
            return SYSCALL_FAILURE;              \
        }                                        \
        p->field##_old = old;                    \
        p->field##_new = new;                    \
    }

syscall_ret_t specPassUp(spu_t type, state_t *old, state_t *new) {
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

syscall_ret_t getPid(pcb_t *p, void **pid, void **ppid) {
    if (pid)
        *pid = p;
    if (ppid)
        *ppid = p->p_parent;
    return SYSCALL_SUCCESS;
}

int *getDeviceSemKey(devreg_t *reg) {
    int line, dev;
    int tmp = ((uint32_t)reg - DEV_REG_START) / DEV_REG_SIZE;
    dev = tmp % N_DEV_PER_IL;
    line = (tmp - dev) / N_DEV_PER_IL + DEV_IL_START;
    return &devices[line][dev];
}