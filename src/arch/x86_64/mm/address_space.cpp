/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "address_space.hpp"

#include <kernel/memory/memory.hpp>
#include <lib/rxx/stdio.hpp>

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
      rxx::printf("Ran out of memory!\n");
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

int X64_AddressSpace::GetX64Flags(int flags) {
  int x64_flags = PT_MAPPED;

  if (flags & AddressSpace::PAGE_WRITABLE)
    x64_flags |= PT_WRITEABLE;

  if (flags & AddressSpace::PAGE_USERSPACE)
    x64_flags |= PT_USERSPACE;

  if (~flags & AddressSpace::PAGE_EXECUTE)
    x64_flags |= PT_NO_EXECUTE;

  return x64_flags;
}

/* TODO: decide how we handle virt/phys addresses that aren't on page boundaries. */

void X64_AddressSpace::Map(vaddr_t virt, paddr_t phys, int flags) {
  int lvl4_idx = ((uint64_t)virt >> 12) & 0x1FF;
  int lvl3_idx = ((uint64_t)virt >> 21) & 0x1FF;
  int lvl2_idx = ((uint64_t)virt >> 30) & 0x1FF;
  int lvl1_idx = ((uint64_t)virt >> 39) & 0x1FF;

  Bind();

  auto pml4 = (ptentry_t *)BuildAddress(256, 256, 256, 256);
  auto pdpt = GetOrCreateTable(pml4, lvl1_idx);
  auto pd = GetOrCreateTable(pdpt, lvl2_idx);
  auto pt = GetOrCreateTable(pd, lvl3_idx);

  /* FIXME: use flags for GetX64Flags. */
  pt[lvl4_idx] = GetX64Flags(PAGE_WRITABLE | PAGE_EXECUTE) | ptentry_t(phys);
}

void X64_AddressSpace::Map(vaddr_t virt, paddr_t phys, size_t size, int flags) {
  auto count = size / 4096;

  if ((size % 4096) != 0)
    count++;

  phys &= ~0xFFF;

  auto virt_s = virt;
  auto virt_e = virt + (count - 1) * 4096;
  auto pml4 = (ptentry_t *)BuildAddress(256, 256, 256, 256);

  int s1 = (virt_s >> 39) & 0x1FF;
  int e1 = (virt_e >> 39) & 0x1FF;

  for (int i = s1; i <= e1; i++) {
    int s2 = 0;
    int e2 = 511;

    if (i == s1)
      s2 = (virt_s >> 30) & 0x1FF;
    if (i == e1)
      e2 = (virt_e >> 30) & 0x1FF;

    auto pdpt = GetOrCreateTable(pml4, i);

    for (int j = s2; j <= e2; j++) {
      int s3 = 0;
      int e3 = 511;

      if (i == s1 && j == s2)
        s3 = (virt_s >> 21) & 0x1FF;
      if (i == e1 && j == e2)
        e3 = (virt_e >> 21) & 0x1FF;

      auto pd = GetOrCreateTable(pdpt, j);

      for (int k = s3; k <= e3; k++) {
        int s4 = 0;
        int e4 = 511;

        if (i == s1 && j == s2 && k == s3)
          s4 = (virt_s >> 12) & 0x1FF;
        if (i == e1 && j == e2 && k == e3)
          e4 = (virt_e >> 12) & 0x1FF;

        auto pt = GetOrCreateTable(pd, k);

        for (int m = s4; m <= e4; m++) {
          /* FIXME: use flags for GetX64Flags. */
          pt[m] = GetX64Flags(PAGE_WRITABLE | PAGE_EXECUTE) | ptentry_t(phys);
          phys += 4096;
        }
      }
    }
  }
}
