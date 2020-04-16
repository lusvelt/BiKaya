#include "init.h"

#include "asl.h"
#include "handler.h"
#include "pcb.h"

HIDDEN void initNewAreas(void) {
    INIT_NEW_AREA(INT_NEWAREA, interruptHandler);
    INIT_NEW_AREA(TLB_NEWAREA, tlbExceptionHandler);
    INIT_NEW_AREA(PGMTRAP_NEWAREA, trapHandler);
    INIT_NEW_AREA(SYSBK_NEWAREA, syscallHandler);
}

void init(void) {
    kernel_enter_tm = 0;
    initNewAreas();
    initPcbs();
    initASL();
}