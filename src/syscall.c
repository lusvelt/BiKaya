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
}