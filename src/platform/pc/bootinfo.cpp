/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <kernel/bootinfo.hpp>
#include <multiboot.hpp>
#include <stdio.hpp>

using namespace multiboot;

static bool convert_mmap(MemoryMap *mmap, kernel::BootInfo *bootinfo) {
  for (auto &entry : *mmap) {
    if (entry.type == MemoryMap::Type::Available) {
      bootinfo->mmap.InsertBack(
          {.base = entry.base, .last = entry.base + entry.length - 1});
    }
  }
  return true;
}

kernel::BootInfo *get_bootinfo(uint32_t magic, void *multiboot) {
  if (magic != multiboot::kMagicNumber) {
    cxx::printf("bootinfo: Kernel was not booted via Multiboot2.\n");
    return nullptr;
  }

  kernel::BootInfo *bootinfo = new kernel::BootInfo();

  for (auto &tag : *(Header *)multiboot) {
    switch (tag.type) {
    case TagType::MemoryMap: {
      convert_mmap((MemoryMap *)&tag, bootinfo);
      break;
    }
    }
  }

  return bootinfo;
}
