#include <stdint.h>

extern "C" void fpu_init();

extern "C" void __cxa_pure_virtual() {
    while (1) { }
}

int printf(const char* format, ...);

extern "C" void kernel_main(void) {
    fpu_init();

    printf("Hello World from the real driver!\n");
    printf("\e[2;32mXXXX is another test! :)\e[0m\rThis\n");
    printf("a\tb\t\tc\td\n");
    printf("Oh dear computer, just what is the answer? 0x%08x\n", 42);

    while (1) { }
}