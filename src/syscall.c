#include "syscall.h"

#include "scheduler.h"
#include "system.h"

syscall_ret_t createProcess(state_t *state, int priority, void **cpid) {
    pcb_t *p = allocPcb();

    if (p == NULL)
        return SYSCALL_FAILURE;

    p->p_s = *state;

    /*
    // TODO: Chiedere se questa parte si può rimuovere, dato che l'inizializzazione è fatta completamente nel test
    uint32_t status = STATUS_GET(&p->p_s);
    // we know status is initialized correctly, due to
    // memset in allocPcb()
    status = STATUS_ALL_INT_ENABLE(status);
    STATUS_SET(&p->p_s, status);
    SET_VM_OFF(&p->p_s);
    SP_SET(&p->p_s, RAM_TOP - FRAME_SIZE);
    p->original_priority = priority;
    p->priority = priority;
    PC_SET(&p->p_s, (memaddr)code);
    */

    p->original_priority = priority;
    p->priority = priority;

    pcb_t *current = getCurrent();
    insertChild(current, p);

    addToReadyQueue(p);

    if (cpid)
        *cpid = p;

    return SYSCALL_SUCCESS;
}

// TODO: Chiedere cosa devo fare con i processi bloccati sui semafori
syscall_ret_t terminateProcess(pcb_t *pid) {
    if (pid == NULL)
        pid = getCurrent();
    if (pid == NULL)
        return SYSCALL_FAILURE;
    removeFromReadyQueue(pid);
    return SYSCALL_SUCCESS;
}

syscall_ret_t verhogen(int *semaddr) {
}

syscall_ret_t passeren(int *semaddr) {
}