#include <stdint.h>
#include <stdio.hpp>
#include <list.hpp>

extern "C" void fpu_init();

extern "C" void __cxa_pure_virtual() {
    while (1) { }
}

struct MyStruct {
    int a;
    int b;
    MyStruct(int a, int b) : a(a), b(b) { }
};

extern "C" void kernel_main(void) {
    fpu_init();

    cxx::printf("Hello World from the real driver!\n");
    cxx::printf("\e[2;32mXXXX is another test! :)\e[0m\rThis\n");
    cxx::printf("a\tb\t\tc\td\n");
    cxx::printf("Oh dear computer, just what is the answer? 0x%08x\n", 42);

    cxx::List<MyStruct> list;

    list.InsertBack({1, 2});
    list.InsertBack({3, 4});

    for (auto& item : list) {
        item.a++;
        item.b--;
    }

    for (auto item : list) {
        cxx::printf("a=%d b=%d\n", item.a, item.b);
    }

    cxx::printf("Done!\n");

    while (1) { }
}
