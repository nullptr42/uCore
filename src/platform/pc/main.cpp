#include <stdint.h>
#include <stdio.hpp>

extern "C" void fpu_init();

extern "C" void __cxa_pure_virtual() {
    while (1) { }
}

extern "C" void kernel_main(void) {
    fpu_init();

    cxx::printf("Hello World from the real driver!\n");
    cxx::printf("\e[2;32mXXXX is another test! :)\e[0m\rThis\n");
    cxx::printf("a\tb\t\tc\td\n");
    cxx::printf("Oh dear computer, just what is the answer? 0x%08x\n", 42);

    while (1) { }
}