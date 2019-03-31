
#include <kernel/bootinfo.hpp>
#include <multiboot.hpp>
#include <stdio.hpp>

using namespace multiboot;

static bool convert_mmap(MemoryMapTag *mmap, kernel::BootInfo *bootinfo) {
  for (auto &entry : *mmap) {
    auto base = entry.base;
    auto last = entry.base + entry.length - 1;

    cxx::printf("0x%016llx -> 0x%016llx\n", base, last);
    bootinfo->mmap.InsertBack({.base = base, .last = last});
  }

  return true;
}

kernel::BootInfo *get_bootinfo(uint32_t magic, void *multiboot) {
  /* Ensure we have been booted via Multiboot2. */
  if (magic != multiboot::kMagicNumber) {
    cxx::printf("bootinfo: Kernel was not booted via Multiboot2.\n");
    return nullptr;
  }

  kernel::BootInfo *bootinfo = new kernel::BootInfo();

  cxx::printf("bootinfo: Multiboot2 header located @ %p.\n", multiboot);
  cxx::printf("bootinfo: Bootinfo structure allocated @ %p.\n", bootinfo);

  for (auto &tag : *(multiboot::Header *)multiboot) {
    switch (tag.type) {
    case multiboot::TagType::MemoryMap: {
      convert_mmap((multiboot::MemoryMapTag *)&tag, bootinfo);
      break;
    }
    }
  }

  cxx::printf("get_bootinfo completed!");

  return bootinfo;
}
