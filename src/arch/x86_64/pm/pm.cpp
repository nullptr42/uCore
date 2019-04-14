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
}

} // namespace arch::x86_64