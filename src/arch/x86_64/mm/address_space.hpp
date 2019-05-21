/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <kernel/memory/virtual/address_space.hpp>
#include <reinix/types.h>

namespace arch::x86_64 {

#define __pgalign __attribute__((aligned(0x1000)))

typedef uint64_t ptentry_t;

class X64_AddressSpace : public kernel::AddressSpace {

public:
  enum EntryFlag {
    PT_MAPPED = (1 << 0),
    PT_WRITEABLE = (1 << 1),
    PT_USERSPACE = (1 << 2),
    PT_WRITE_THROUGH = (1 << 3),
    PT_DISABLE_CACHE = (1 << 4),
    PT_WAS_USED = (1 << 5),
    PT_HUGE_PAGES = (1 << 7),
    PT_NO_EXECUTE = (1LL << 63)
  };

  X64_AddressSpace() { /* ... */
  }

  X64_AddressSpace(uint64_t *pml4) : pml4(pml4) {}

  void Map(vaddr_t virt, paddr_t phys, int flags) final;
  void Map(vaddr_t virt, paddr_t phys, size_t size, int flags) final;

  void Bind() {
    /* TODO: check if CR3 is up to date. */
    asm("mov %0, %%cr3\n" : : "r"(pml4));
  }

private:
  static ptentry_t *GetChildAddress(ptentry_t *parent, int child);
  static ptentry_t *GetOrCreateTable(ptentry_t *parent, int child);
  
  static int GetX64Flags(int flags);
  
  constexpr vaddr_t BuildAddress(int pml4, int pdpt, int pd, int pt) {
    return vaddr_t(
        ((vaddr_t)pml4 << 39) |
        ((vaddr_t)pdpt << 30) |
        ((vaddr_t)pd   << 21) |
        ((vaddr_t)pt   << 12) |
        0xFFFF000000000000ULL
    );
  }

  ptentry_t *pml4;
};

} // namespace arch::x86_64