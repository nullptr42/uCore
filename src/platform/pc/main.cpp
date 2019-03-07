#include <stdint.h>
#include <platform/print.hpp>

extern "C" void fpu_init();

extern "C" void __cxa_pure_virtual() {
    while (1) { }
}

extern "C" void kernel_main(void) {
    fpu_init();

    platform::print("Hello World");

    while (1) { }
}