#include "init.h"
#include "scheduler.h"
#include "term.h"

extern void test1();
extern void test2();
extern void test3();

int main(void) {
    init();

    createProcess(test1, 1);

    start();
}