#include "init.h"
#include "scheduler.h"
#include "terminal.h"
#include "utils.h"

extern void test1();
extern void test2();
extern void test3();

int main(void) {
    init();

    createPcb(test1, 1);

    start();
    return 0;
}