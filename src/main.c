#include "init.h"
#include "scheduler.h"
#include "terminal.h"
#include "utils.h"

extern void test1();
extern void test2();
extern void test3();

int main(void) {
    init();

    if (createPcb(test1, 1) == ERR_NO_PROC) {
        println("Maximum PCB allocations reached");
        PANIC();
    }

    start();
    return 0;
}