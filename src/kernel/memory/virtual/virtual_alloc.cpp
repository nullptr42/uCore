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
    : last(last), granularity(granularity) {
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

vaddr_t VirtualRangeAllocator::Free(vaddr_t base, size_t size) {
  vaddr_t result = 0;

  /* TODO: check if base should round up and size round down or vice versa.
   * Right now I can see arguments for both.
   */

  /* Round base down to the nearest page. */
  if ((base % granularity) != 0)
    base -= (base % granularity);

  /* Round size up to the nearest page. */
  if ((size % granularity) != 0)
    size += granularity - (size % granularity);

  vaddr_t last = base + size - 1;

  for (auto it = ranges.begin(); it != ranges.end(); ++it) {
    // TODO: this is ugly because we have to work on the internal ListNode
    // structure itself.
    auto node = it.position;
    auto next = node->next;
    vaddr_t max = next ? next->value.base : this->last;
    if (base > node->value.last && last < max) {
      MemoryRange range = {base, last};
      result = base;

      /* Try to merge with the next block. */
      if (next && (range.last + 1) == next->value.base) {
        range.last = next->value.last;
        ranges.Erase(it + 1);
      }

      /* Try to merge into the previous block. Otherwise insert the new range.
       */
      if ((range.base - 1) == node->value.last) {
        node->value.last = range.last;
      } else {
        ranges.InsertAfter(it, range);
      }
      break;
    }
  }

  // debug ranges
  for (auto &range : ranges) {
    rxx::printf("base=0x%016llx last=0x%016llx\n", range.base, range.last);
  }

  return result;
}