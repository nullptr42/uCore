#include <stdint.h>

static uint16_t* buffer = (uint16_t*)0xFFFFFFFF800B8000;

extern "C" void kernel_main(void) {
    buffer[0] = 0x0741;
    buffer[1] = 0x0742;
    while (1) { }
}