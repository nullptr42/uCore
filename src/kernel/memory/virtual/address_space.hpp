/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <lib/rxx/array.hpp>
#include <reinix/types.h>

namespace kernel {

class AddressSpace {

public:
  enum Flag {
    // PAGE_READABLE = 1 << 0,
    PAGE_WRITABLE = 1 << 1,
    PAGE_EXECUTE = 1 << 2,
    PAGE_USERSPACE = 1 << 3,
    PAGE_RW = PAGE_WRITABLE,
    PAGE_RWX = PAGE_RW | PAGE_EXECUTE
  };

  virtual void Map(vaddr_t virt, paddr_t phys, int flags) = 0;
  virtual void Map(vaddr_t virt, paddr_t phys, size_t size, int flags) = 0;
  virtual void Map(vaddr_t virt, const rxx::Array<page_t> &pages,
                   int flags) = 0;
  virtual void Unmap(vaddr_t virt, size_t size) = 0;
  virtual void Protect(vaddr_t virt, size_t size, int flags) = 0;
  /* ... */
};

} // namespace kernel