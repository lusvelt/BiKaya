#include "init.h"
#include "scheduler.h"
#include "terminal.h"
#include "utils.h"

extern void test();

int main(void) {
    init();

    start();

    return 0;
}