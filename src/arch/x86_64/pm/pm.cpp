/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "pm.hpp"
#include "../global.hpp"
#include <lib/rxx/stdio.hpp>
#include <reinix/types.h>

extern "C" {
// TODO: C++ does not allow use of the void type here.
extern const uint8_t kernel_start;
extern const uint8_t kernel_end;
}

namespace arch::x86_64 {

PhysicalMemoryAllocator::PhysicalMemoryAllocator(kernel::BootInfo *bootinfo) {
  paddr_t minimum = physical((vaddr_t)&kernel_end) + 1;

  /* Determine the lowest safe memory address. */
  for (auto &mod : bootinfo->modules) {
    if (mod.last > minimum) {
      minimum = mod.last + 1;
    }
  }

  minimum = round_up(minimum);
  rxx::printf("[pm] minimum=0x%016x\n", minimum);

  uint64_t usable = 0;

  /* Count the amount of usable RAM. */
  for (auto &entry : bootinfo->mmap) {
    usable += entry.last - entry.base + 1;
  }

  usable = round_down(usable);
  rxx::printf("[pm] detected %lld bytes of available memory.\n", usable);

  capacity = pagecount(usable);

  /* Search for a suitable location for the page stack. */
  for (auto &entry : bootinfo->mmap) {
    auto base = entry.base;

    if (base < minimum)
      base = minimum;
    if (base > entry.last)
      continue;

    entry.base = base;

    if ((entry.last - base + 1) >= sizeof(uint64_t) * capacity) {
      entry.base += sizeof(uint64_t) * capacity;
      pages = (uint64_t *)base;
      break;
    }
  }

  rxx::printf("[pm] page frame stack allocated @ 0x%016llx.\n", pages);

  /* Release all free pages into the stack. */
  for (auto &entry : bootinfo->mmap) {
    auto base = page(round_up(entry.base));
    auto last = page(round_down(entry.last));

    for (uint64_t pg = base; pg <= last; pg++) {
      Free(1, &pg);
    }
  }
}

auto PhysicalMemoryAllocator::Alloc(size_t count, uint64_t *pages) -> Status {
  if (count > index)
    return Status::OutOfMemory;

  index -= count;
  for (size_t i = 0; i < count; i++)
    pages[i] = this->pages[index + i];

  return Status::OK;
}

auto PhysicalMemoryAllocator::Free(size_t count, uint64_t *pages) -> Status {
  if ((index + count) > capacity)
    return Status::BadRequest;

  for (size_t i = 0; i < count; i++)
    this->pages[index + i] = pages[i];
  index += count;

  return Status::OK;
}

} // namespace arch::x86_64