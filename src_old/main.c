#include "init.h"
#include "scheduler.h"
#include "syscall.h"
#include "terminal.h"

extern void test();

int main(void) {
    //init();

    //     state_t s;
    //     STST(&s);

    // #ifdef TARGET_UMPS
    //     s.reg_sp = RAM_TOP - FRAME_SIZE;
    //     s.pc_epc = test;
    //     s.status = STATUS_ALL_INT_ENABLE(s.status);
    //     s.status = SET_KERNEL_MODE(s.status);
    // #endif

    // #ifdef TARGET_UARM
    //     s.sp = RAM_TOP - FRAME_SIZE;
    //     s.pc = test;
    //     s.cpsr = STATUS_ALL_INT_ENABLE(s.cpsr);
    //     s.cpsr = SET_KERNEL_MODE(s.cpsr);
    // #endif
    //     SET_VM_OFF(&s);
    //     pcb_t *p = pcb_alloc();

    //     p->p_s = s;

    //     p->original_priority = 1;
    //     p->priority = 1;

    //addToReadyQueue(p);

    return scheduler_start(test);
}