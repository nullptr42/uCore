/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <reinix/types.h>

namespace arch::x86_64 {

constexpr vaddr_t kKernelVirtualBase = 0xFFFFFFFF80000000;

constexpr paddr_t physical(vaddr_t _virtual) {
  return _virtual - kKernelVirtualBase;
}

constexpr addr_t round_up(addr_t address) {
  if ((address % 4096) != 0) {
    address += 4096 - (address % 4096);
  }

  return address;
}

constexpr addr_t round_down(addr_t address) {
  if ((address % 4096) != 0) {
    address -= address % 4096;
  }

  return address;
}

constexpr size_t pagecount(uint64_t size) { return size / 4096; }

constexpr uint64_t page(addr_t address) { return address / 4096; }

} // namespace arch::x86_64