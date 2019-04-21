/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "virtual_alloc.hpp"

using namespace kernel;

VirtualRangeAllocator::VirtualRangeAllocator(vaddr_t base, vaddr_t last) {
  ranges.InsertBack({base, last});
}

vaddr_t VirtualRangeAllocator::Alloc(vaddr_t base, size_t size) {
  vaddr_t last = base + size - 1;
  vaddr_t result = 0;

  for (auto &range : ranges) {
    size_t available = range.last - range.base + 1;

    if (available < size)
      continue;

    if (range.base >= base) {
      result = range.base;
      range.base += size;
      // TODO: get rid of empty ranges.
      break;
    }

    if (range.last >= last) {
      // TODO: split range to create a hole.
    }
  }

  return result;
}

vaddr_t VirtualRangeAllocator::Free(vaddr_t base, size_t size) { return base; }