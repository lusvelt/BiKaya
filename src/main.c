#include "asl.h"
#include "macro.h"
#include "pcb.h"
#include "scheduler.h"
#include "system.h"

extern void test();

int main(void) {
    INIT_NEW_AREA(INT_NEWAREA, handler_interrupt);
    INIT_NEW_AREA(TLB_NEWAREA, handler_tlb_exception);
    INIT_NEW_AREA(PGMTRAP_NEWAREA, handler_trap);
    INIT_NEW_AREA(SYSBK_NEWAREA, handler_syscall);

    pcb_init();
    asl_init();

    scheduler_init(test);

    return 0;
}