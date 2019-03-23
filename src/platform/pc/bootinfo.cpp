
#include <stdio.hpp>
#include <multiboot2.hpp>
#include <kernel/bootinfo.hpp>

static bool callback_mmap(mb2_tag* tag, kernel::BootInfo* bootinfo) {
    auto mmap = (mb2_mmap_tag*)tag;
    auto size = uint32_t(sizeof(mb2_mmap_tag));
    mb2_mmap_ent* entry = (mb2_mmap_ent*)((void*)mmap + size);

    /* Number of entries is defined through entry size and total structure size. */
    while (size < tag->size) {
        auto base = entry->base;
        auto last = entry->base + entry->length - 1;

        cxx::printf("0x%016llx -> 0x%016llx\n", base, last);
        bootinfo->mmap.InsertBack({ .base = base, .last = last });

        /* Update size and entry address. */
        size += mmap->entry_size;
        entry = (mb2_mmap_ent*)((void*)entry + mmap->entry_size);
    }

    return true;
}

kernel::BootInfo* get_bootinfo(uint32_t magic, void* multiboot) {
    /* Ensure we have been booted via Multiboot2. */
    if (!multiboot2_verify(magic)) {
        cxx::printf("bootinfo: Kernel was not booted via Multiboot2.\n");
        return nullptr;
    }
    
    kernel::BootInfo* bootinfo = new kernel::BootInfo();

    cxx::printf("bootinfo: Multiboot2 header located @ %p.\n", multiboot);
    cxx::printf("bootinfo: Bootinfo structure allocated @ %p.\n", bootinfo);

    multiboot2_find_tags((mb2_info*)multiboot, MB_TAG_MMAP, (tag_handler)callback_mmap, bootinfo);

    return bootinfo;
}
