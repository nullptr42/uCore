/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <kernel/bootinfo.hpp>
#include <stdint.h>
#include <stddef.h>

namespace arch::x86_64 {

class PhysicalMemoryAllocator {
  uint64_t *pages = nullptr;
  size_t capacity;
  size_t index = 0;

public:
  enum class Status {
    BadRequest = -2,
    OutOfMemory = -1,
    OK = 0
  };

  PhysicalMemoryAllocator(kernel::BootInfo *bootinfo);

  auto Alloc(size_t count, uint64_t *pages) -> Status;
  auto Free(size_t count, uint64_t *pages) -> Status;
};

} // namespace arch::x86_64