#include "syscall.h"

#include "asl.h"
#include "scheduler.h"
#include "system.h"

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

    if (list_empty(semd->s_procQ))
        *semaddr++;
    else
        addToReadyQueue(removeBlocked(semaddr));
    return SYSCALL_SUCCESS;
}

syscall_ret_t passeren(int *semaddr, pcb_t *pid) {
    semd_t *semd = getSemd(semaddr);

    if (*semaddr)
        *semaddr--;
    else
        insertBlocked(semaddr, pid);
    return SYSCALL_SUCCESS;
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
        *pid = current;
    if (ppid)
        *ppid = current->p_parent;
    return SYSCALL_SUCCESS;
}