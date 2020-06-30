#include "arch.h"
#include "asl.h"
#include "exceptions.h"
#include "interrupts.h"
#include "macro.h"
#include "memory.h"
#include "pcb.h"
#include "scheduler.h"
#include "syscalls.h"

extern void test();

int main(void) {
    // At startup

    // 1. Initialize kernel new areas
    INIT_NEW_AREA((state_t *)INT_NEWAREA, interrupts_handler);
    INIT_NEW_AREA((state_t *)TLB_NEWAREA, tlb_exception_handler);
    INIT_NEW_AREA((state_t *)PGMTRAP_NEWAREA, trap_exception_handler);
    INIT_NEW_AREA((state_t *)SYSBK_NEWAREA, syscall_handler);

    // 2. Initialize pcbs and asl structures
    pcb_init();
    asl_init();

    // 3. Initialize scheduler structures
    scheduler_init(test);

    // 4. Start the scheduler
    scheduler_run();

    return 0;
}