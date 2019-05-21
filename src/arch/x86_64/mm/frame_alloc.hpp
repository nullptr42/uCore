/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <kernel/bootinfo.hpp>
#include <kernel/memory/physical/frame_alloc.hpp>
#include <stddef.h>
#include <stdint.h>

namespace arch::x86_64 {

class X64_AddressSpace;

class X64_FrameAllocator : public kernel::FrameAllocator {
  page_t *pages = nullptr;
  size_t capacity;
  size_t index = 0;

public:
  X64_FrameAllocator(kernel::BootInfo *bootinfo, X64_AddressSpace &aspace);

  auto Alloc(rxx::Array<page_t> &pages, int flags) -> Status final;
  auto Free(rxx::Array<page_t> &pages) -> Status final;
};

} // namespace arch::x86_64