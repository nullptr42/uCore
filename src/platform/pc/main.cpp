#include <stdint.h>
#include <stdio.hpp>
#include <list.hpp>
#include <arch/x86_64/gdt.hpp>
#include <kernel/version.hpp>

extern "C" void fpu_init();

void idt_init();

extern "C" void __cxa_pure_virtual() {
    while (1) { }
}

extern "C" void kernel_main(void) {
    fpu_init();

    /* Setup flat segmentation and interrupt vector table. */
    gdt_init();
    idt_init();

    cxx::printf("\e[2;37m%s\e[0m %d.%d\n\n",
        kernel::g_kernel_info.name,
        kernel::g_kernel_info.version.major,
        kernel::g_kernel_info.version.minor
    );

    //asm("sti");
    for (;;) asm("hlt");
}
