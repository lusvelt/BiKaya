#include "utils.h"

#include "scheduler.h"
#include "system.h"

err_t createPcb(pcb_handler_t func, uint8_t n) {
    pcb_t *p = allocPcb();

    if (p == NULL)
        return ERR_NO_PROC;

    uint32_t status = STATUS_GET(&p->p_s);
    status = STATUS_ALL_INT_DISABLE(status) | STATUS_ENABLE_TIMER(status) | SET_KERNEL_MODE(status);
    STATUS_SET(&p->p_s, status);
    SET_VM_OFF(&p->p_s);
    SP_SET(&p->p_s, RAM_TOP - FRAME_SIZE * n);
    p->original_priority = n;
    p->priority = n;
    PC_SET(&p->p_s, (memaddr)func);

    addToReadyQueue(p);

    return OK;
}
