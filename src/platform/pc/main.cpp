#include <stdint.h>
#include <platform/print.hpp>

extern "C" void fpu_init();

extern "C" void __cxa_pure_virtual() {
    while (1) { }
}

extern "C" void kernel_main(void) {
    fpu_init();

    platform::print("Hello World from the real driver!\n");
    platform::print("\e[2;32mXXXX is another test! :)\e[0m\rThis\n");
    platform::print("a\tb\t\tc\td\n");

    for (int i = 0; i < 20; i++) {
        platform::print("xoobar\rf\n");
    }

    for (int i = 0; i < 81; i++) {
        platform::print("y");
    }

    platform::print("\nDone printing!");

    while (1) { }
}