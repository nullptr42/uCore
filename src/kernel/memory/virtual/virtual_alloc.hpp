/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <lib/rxx/list.hpp>
#include <reinix/types.h>

namespace kernel {

// TODO: How do we handle something like VirtualQuery(...)?

class VirtualRangeAllocator {
  struct MemoryRange {
    vaddr_t base;
    vaddr_t last;
  };

  vaddr_t last;
  int granularity;

  rxx::List<MemoryRange> ranges;

public:
  VirtualRangeAllocator(vaddr_t base, vaddr_t last, int granularity);

  vaddr_t Alloc(vaddr_t base, size_t size);
  vaddr_t Free(vaddr_t base, size_t size);
};

} // namespace kernel

// address range allocator