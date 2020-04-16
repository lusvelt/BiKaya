#include "init.h"
#include "scheduler.h"
#include "terminal.h"
#include "utils.h"

extern void test();

int main(void) {
    init();

    if (createProcess(test, DEFAULT_PRIORITY) == ERR_NO_PROC)
        EXIT("Maximum PCB allocations reached");

    start();

    return 0;
}