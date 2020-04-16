#include "utils.h"

#include "scheduler.h"
#include "system.h"

err_t createProcess(pcb_code_t code, uint8_t priority) {
    pcb_t *p = allocPcb();

    if (p == NULL)
        return ERR_NO_PROC;

    uint32_t status = STATUS_GET(&p->p_s);
    // we know status is initialized correctly, due to
    // memset in allocPcb()
    status = STATUS_DISABLE_INT(status) | STATUS_ENABLE_TIMER(status) | SET_KERNEL_MODE(status);
    STATUS_SET(&p->p_s, status);
    SET_VM_OFF(&p->p_s);
    SP_SET(&p->p_s, RAM_TOP - FRAME_SIZE * priority);
    p->original_priority = priority;
    p->priority = priority;
    PC_SET(&p->p_s, (memaddr)code);

    addToReadyQueue(p);

    return OK;
}
