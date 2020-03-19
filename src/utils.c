#include "utils.h"

#include "scheduler.h"

pcb_t *createPcb(pcb_handler_t func, uint8_t n) {
    pcb_t *p = allocPcb();

    if (p == NULL)
        return NULL;
    p->p_s.cpsr = STATUS_ENABLE_INT(STATUS_SYS_MODE);
    p->p_s.CP15_Control = CP15_DISABLE_VM(CP15_CONTROL_NULL);
    p->p_s.sp = RAM_TOP - FRAME_SIZE * n;
    p->original_priority = n;
    p->priority = n;
    p->p_s.pc = (memaddr)func;

    addToReadyQueue(p);

    return p;
}