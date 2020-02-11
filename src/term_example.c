#include "term.h"

/** 
 * term.h functions come in two flavors:
 * - without 't'-prefix, e.g. putchar, puts, print, println, getchar, gets
 * - with 't'-prefix, e.g. tputchar, tputs, tprint, tgetchar, tgets. 
 * The only difference between the two types is that 't'-prefixed functions 
 * take a pointer to a terminal register (termreg_t* type) as 1st argument
 * while the others don't. So, prefixed functions could be used to print
 * to any terminal while unprefixed ones default to terminal 0.
 */
int main(void) {
    // To get a terminal instance you can use TERM(number) macro
    termreg_t *term1 = TERM(1);
    termreg_t *term2 = TERM(2);
    termreg_t *term0 = TERM_0;  // Useful alias for TERM(0)

    putchar('0');          // Prints 0 to terminal 0
    tputchar(term1, '1');  // Prints 1 to terminal 1
    tputchar(term2, '2');  // Prints 2 to terminal 2

    puts("Hello from terminal 0\n");          // Prints "Hello from terminal 0" to terminal 0
    tputs(term1, "Hello from terminal 1\n");  // Prints "Hello from terminal 1" to terminal 1
    tputs(term2, "Hello from terminal 2\n");  // Prints "Hello from terminal 2" to terminal 2

    println("Hello from terminal %d\n", 0);
    tprintf(term1, "Hello from terminal %d\n", 1);
    tprintf(term2, "Hello from terminal %d\n", 2);

    println("Hello from terminal %c\n", '0');
    tprintf(term1, "Hello from terminal %c\n", '1');
    tprintf(term2, "Hello from terminal %c\n", '2');

    println("Hello from %s\n", "terminal 0");
    tprintf(term1, "Hello from %s\n", "terminal 1");
    tprintf(term2, "Hello from %s\n", "terminal 2");

    println("Hello from terminal %d with address %p\n", 0, (void *)term0);
    tprintf(term1, "Hello from terminal %d with address %p\n", 1, (void *)term1);
    tprintf(term2, "Hello from terminal %d with address %p\n", 2, (void *)term2);

    return 0;
}