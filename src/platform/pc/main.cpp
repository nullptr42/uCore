#include <stdint.h>

static uint16_t* buffer = (uint16_t*)0xFFFFFFFF800B8000;

extern "C" void fpu_init();

struct TestClass {
    int x;
    int y;

    TestClass(int x, int y) : x(x), y(y) { }
};

extern "C" void kernel_main(void) {
    fpu_init();

    TestClass* test = new TestClass(123, 456);
    
    buffer[0] = 0x0741;
    buffer[1] = 0x0742;
    while (1) { }
}