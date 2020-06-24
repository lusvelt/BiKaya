#include "exceptions.h"

#include "const.h"
#include "memory.h"
#include "syscall.h"
#include "types.h"

extern pcb_t *current_proc;

void trap_exception_handler(void) {
    if (current_proc->exc_new_areas[SPECPASSUP_TRAP_TYPE]) {
        memcpy(current_proc->exc_old_areas[SPECPASSUP_TRAP_TYPE], &current_proc->p_s, sizeof(state_t));
        LDST(current_proc->exc_new_areas[SPECPASSUP_TRAP_TYPE]);
    } else {
        terminate_process(current_proc);
    }
}

void tlb_exception_handler(void) {
    if (current_proc->exc_new_areas[SPECPASSUP_TLB_TYPE]) {
        memcpy(current_proc->exc_old_areas[SPECPASSUP_TLB_TYPE], &current_proc->p_s, sizeof(state_t));
        LDST(current_proc->exc_new_areas[SPECPASSUP_TLB_TYPE]);
    } else {
        terminate_process(current_proc);
    }
}