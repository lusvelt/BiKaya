#include "utils.h"

#include "scheduler.h"

pcb_t *createPcb(pcb_handler_t func, uint8_t n) {
    pcb_t *p = allocPcb();

    if (p == NULL)
        return NULL;
    uint32_t status = STATUS_GET(&p->p_s);
    status = STATUS_ENABLE_INT(status);
    status = SET_KERNEL_MODE(status);
    STATUS_SET(&p->p_s, status);
    SET_VM_OFF(&p->p_s);
    SP_SET(&p->p_s, RAM_TOP - FRAME_SIZE * n);
    p->original_priority = n;
    p->priority = n;
    PC_SET(&p->p_s, (memaddr)func);

    addToReadyQueue(p);

    return p;
}