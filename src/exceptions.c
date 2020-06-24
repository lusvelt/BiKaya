#include "exceptions.h"

#include "const.h"
#include "memory.h"
#include "scheduler.h"
#include "syscall.h"
#include "types.h"

extern pcb_t *current_proc;

void trap_exception_handler(void) {
    state_t *old_state = (state_t *)PGMTRAP_OLDAREA;

    if (current_proc->exc_new_areas[SPECPASSUP_TRAP_TYPE]) {
        memcpy(current_proc->exc_old_areas[SPECPASSUP_TRAP_TYPE], old_state, sizeof(state_t));
        LDST(current_proc->exc_new_areas[SPECPASSUP_TRAP_TYPE]);
    } else {
        terminate_process(current_proc);
        scheduler_resume(FALSE, NULL);
    }
}

void tlb_exception_handler(void) {
    state_t *old_state = (state_t *)TLB_OLDAREA;

    if (current_proc->exc_new_areas[SPECPASSUP_TLB_TYPE]) {
        memcpy(current_proc->exc_old_areas[SPECPASSUP_TLB_TYPE], old_state, sizeof(state_t));
        LDST(current_proc->exc_new_areas[SPECPASSUP_TLB_TYPE]);
    } else {
        terminate_process(current_proc);
        scheduler_resume(FALSE, NULL);
    }
}