/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "address_space.hpp"

#include <kernel/memory/memory.hpp>

using namespace arch::x86_64;

ptentry_t *X64_AddressSpace::GetChildAddress(ptentry_t *parent, int child) {
  return (ptentry_t *)((uint64_t(parent) << 9) | (child << 12) |
                       0xFFFF000000000000ULL);
}

ptentry_t *X64_AddressSpace::GetOrCreateTable(ptentry_t *parent, int child) {
  auto table = GetChildAddress(parent, child);

  /* Create table if it doesn't exist yet. */
  if (~parent[child] & PT_MAPPED) {
    page_t page_new;
    rxx::Array<page_t> hack{&page_new, 1};

    if (kernel::g_frame_alloc->Alloc(hack, 0) !=
        kernel::FrameAllocator::Status::Success) {
      /* TODO: we ran out of memory, maybe we should panic.. */
      while (1)
        ;
    }

    /* Map table in parent table and clear its contents. */
    parent[child] = PT_MAPPED | PT_WRITEABLE | ptentry_t(page_new * 4096);
    for (int i = 0; i < 512; i++)
      table[i] = 0;
  }

  return table;
}

void X64_AddressSpace::Map(vaddr_t virt, paddr_t phys, int flags) {
  int lvl4_idx = ((uint64_t)virt >> 12) & 0x1FF;
  int lvl3_idx = ((uint64_t)virt >> 21) & 0x1FF;
  int lvl2_idx = ((uint64_t)virt >> 30) & 0x1FF;
  int lvl1_idx = ((uint64_t)virt >> 39) & 0x1FF;
  
  Bind();
  
  auto pml4 = (ptentry_t*)BuildAddress(256, 256, 256, 256);
  auto pdpt = GetOrCreateTable(pml4, lvl1_idx);
  auto pd = GetOrCreateTable(pdpt, lvl2_idx);
  auto pt = GetOrCreateTable(pd, lvl3_idx);
  
  /* TODO: convert flags to x86_64 flags. */
  pt[lvl4_idx] = PT_MAPPED | PT_WRITEABLE | ptentry_t(phys);
}

void X64_AddressSpace::Map(vaddr_t virt, paddr_t phys, size_t size, int flags) {
  /* TODO: implement this properly. */
  
  auto count = size / 4096;
  
  if ((size % 4096) != 0) count++;
  
  phys &= ~0xFFF;
  
  for (size_t i = 0; i < count; i++) {
    Map(virt, phys, flags);
    virt += 4096;
    phys += 4096;
  }
}
