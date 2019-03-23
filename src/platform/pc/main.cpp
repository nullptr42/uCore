#include <stdint.h>
#include <stdio.hpp>
#include <list.hpp>
#include <arch/x86_64/gdt.hpp>
#include <arch/x86_64/idt.hpp>
#include <arch/x86_64/pic.hpp>
#include <kernel/version.hpp>
#include <kernel/bootinfo.hpp>

extern "C" void fpu_init();

using namespace arch::x86_64;

extern "C" void __cxa_pure_virtual() {
    while (1) { }
}

kernel::BootInfo* g_bootinfo = nullptr;

kernel::BootInfo* get_bootinfo(uint32_t magic, void* multiboot);

extern "C" void kernel_main(uint32_t magic, void* multiboot) {
    fpu_init();

    /* Setup flat segmentation and interrupt vector table. */
    gdt::initialize();
    idt::initialize();
    pic::initialize();

    cxx::printf("\e[2;37m%s\e[0m %d.%d\n\n",
        kernel::g_kernel_info.name,
        kernel::g_kernel_info.version.major,
        kernel::g_kernel_info.version.minor
    );

    pic::set_mask(0);

    g_bootinfo = get_bootinfo(magic, multiboot);

    asm("sti");
    for (;;) asm("hlt");
}
