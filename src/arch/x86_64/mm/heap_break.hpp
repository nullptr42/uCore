/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include "address_space.hpp"
#include <kernel/memory/heap_break.hpp>
#include <lib/rxx/array.hpp>
#include <reinix/types.h>

namespace arch::x86_64 {

class X64_HeapBreak : public kernel::HeapBreak {
  void *base = (void *)0xFFFF808000000000;
  void *last = (void *)0xFFFFFFFF00000000;
  void *curr = base;

  X64_AddressSpace &aspace;

public:
  X64_HeapBreak(X64_AddressSpace &aspace) : aspace(aspace) {}

  void *Grow(size_t size) final {
    size = round_up(size);

    if ((curr + size) > last) {
      return nullptr;
    }

    int count = size / 4096;
    page_t page;
    rxx::Array<page_t> pages{&page, 1};

    /* TODO: this is total crap and needs to be fixed... */
    for (int i = 0; i < count; i++) {
      if (kernel::g_frame_alloc->Alloc(pages, 0) !=
          kernel::FrameAllocator::Status::Success) {
        rxx::printf("Ran out of memory!\n");
        while (1)
          ;
      }
      
      aspace.Map(vaddr_t(curr), page*4096, 0);
      curr += 4096;
    }

    return curr;
  }

  void *Shrink(size_t size) final {
    //size = round_down(size);
    return curr;
  }
};

}