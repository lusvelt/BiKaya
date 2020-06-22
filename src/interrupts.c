#include "interrupts.h"

#include "const.h"
#include "memory.h"
#include "types.h"

HIDDEN devices_semkeys[N_EXT_IL][N_DEV_PER_IL];

void interrupts_handler(void) {
    state_t old_state = *((state_t *)INT_OLDAREA);
    uint32_t cause = CAUSE(old_state);

#ifdef TARGET_UARM
    PC(old_state) -= WORD_SIZE;
#endif

    if (INT_IS_PENDING(cause, IL_TIMER)) {
    }
}

int *interrupts_get_dev_key(devreg_t *reg) {
    int tmp = ((uint32_t)reg - DEV_REG_START) / DEV_REG_SIZE;
    int dev_index = tmp % N_DEV_PER_IL;
    return &devices_semkeys[dev_index][(tmp - dev_index) / N_DEV_PER_IL + DEV_IL_START];
}