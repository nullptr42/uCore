
#include <stdio.hpp>
#include "multiboot.hpp"

static const uint32_t kMultiboot2Magic = 0x36D76289;

kernel::BootInfo* get_bootinfo(uint32_t magic, void* _multiboot) {
    /* Ensure we have been booted via Multiboot2. */
    if (magic != kMultiboot2Magic) {
        cxx::printf("bootinfo: Kernel was not booted via Multiboot2.\n");
        return nullptr;
    }
    kernel::BootInfo* bootinfo = new kernel::BootInfo();

    cxx::printf("bootinfo: Multiboot2 header located @ %p.\n", _multiboot);
    cxx::printf("bootinfo: Bootinfo structure allocated @ %p.\n", bootinfo);

    return bootinfo;
}