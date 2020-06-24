#include "arch.h"
#include "asl.h"
#include "exceptions.h"
#include "interrupts.h"
#include "macro.h"
#include "memory.h"
#include "pcb.h"
#include "scheduler.h"
#include "syscall.h"

extern void test();

int main(void) {
    INIT_NEW_AREA((state_t *)INT_NEWAREA, interrupts_handler);
    INIT_NEW_AREA((state_t *)TLB_NEWAREA, tlb_exception_handler);
    INIT_NEW_AREA((state_t *)PGMTRAP_NEWAREA, trap_exception_handler);
    INIT_NEW_AREA((state_t *)SYSBK_NEWAREA, syscalls_handler);

    pcb_init();
    asl_init();

    scheduler_init(test);

    scheduler_run();

    return 0;
}