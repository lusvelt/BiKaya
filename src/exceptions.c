#include "exceptions.h"

#include "const.h"
#include "scheduler.h"
#include "types.h"

void trap_exception_handler(void) {
    // Simply delegates to the scheduler passing the right exception type
    scheduler_handle_exception(SPECPASSUP_TRAP_TYPE, (state_t *)PGMTRAP_OLDAREA);
}

void tlb_exception_handler(void) {
    // Simply delegates to the scheduler passing the right exception type
    scheduler_handle_exception(SPECPASSUP_TLB_TYPE, (state_t *)TLB_OLDAREA);
}