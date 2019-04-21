/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "virtual_alloc.hpp"

#include <lib/rxx/stdio.hpp>

using namespace kernel;

VirtualRangeAllocator::VirtualRangeAllocator(vaddr_t base, vaddr_t last,
                                             int granularity)
    : granularity(granularity) {
  ranges.InsertBack({base, last});
}

vaddr_t VirtualRangeAllocator::Alloc(vaddr_t base, size_t size) {
  vaddr_t result = 0;

  /* Round base down to the nearest page. */
  if ((base % granularity) != 0)
    base -= (base % granularity);

  /* Round size up to the nearest page. */
  if ((size % granularity) != 0)
    size += granularity - (size % granularity);

  vaddr_t last = base + size - 1;

  for (auto it = ranges.begin(); it != ranges.end(); ++it) {
    auto &range = *it;
    size_t available = range.last - range.base + 1;

    /* If the current range is too small don't bother with it. */
    if (available < size)
      continue;

    /* If the range base address already is higher than
     * the requested base address just allocate space from
     * the start of this range.
     */
    if (range.base >= base) {
      result = range.base;
      range.base += size;
      /* Remove the range if its size is zero after the allocation. */
      if (range.base > range.last)
        ranges.Erase(it);
      break;
    }

    /* The range base address is lower than the requested base address
     * and is large enough. The requested range is fully contained.
     */
    if (range.last >= last) {
      result = base;
      /* Create a free range following the hole if there is free space
       * following the allocated range.
       */
      if (range.last > last)
        ranges.InsertAfter(it, {last + 1, range.last});
      range.last = base - 1;
      break;
    }
  }

  // debug ranges
  for (auto &range : ranges) {
    rxx::printf("base=0x%016llx last=0x%016llx\n", range.base, range.last);
  }

  return result;
}

vaddr_t VirtualRangeAllocator::Free(vaddr_t base, size_t size) { return base; }