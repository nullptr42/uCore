/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <kernel/bootinfo.hpp>
#include <stdint.h>

namespace arch::x86_64 {

class PhysicalMemoryAllocator {
  uint64_t *pages = nullptr;
  int capacity;
  int index = 0;

public:
  PhysicalMemoryAllocator(kernel::BootInfo *bootinfo);
};

} // namespace arch::x86_64