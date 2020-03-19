#include "init.h"
#include "term.h"
#include "utils.h"

extern void test1();
extern void test2();
extern void test3();

int main(void) {
    init();

    createPcb(test1, 1);
    createPcb(test2, 2);
    createPcb(test3, 3);
}