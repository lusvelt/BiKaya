#include "init.h"

#include "asl.h"
#include "handler.h"
#include "pcb.h"

HIDDEN void initNewAreas(void) {
    INIT_NEW_AREA(INT_NEWAREA, handler_interrupt);
    INIT_NEW_AREA(TLB_NEWAREA, handler_tlb_exception);
    INIT_NEW_AREA(PGMTRAP_NEWAREA, handler_trap);
    INIT_NEW_AREA(SYSBK_NEWAREA, handler_syscall);
}

void init(void) {
    initNewAreas();
    pcb_init();
    asl_init();
}