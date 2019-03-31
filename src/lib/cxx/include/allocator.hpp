/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <stddef.h>

namespace cxx {

/**
 * Generic interface for memory allocation.
 */
struct Allocator {
  virtual void *Alloc(size_t size) = 0;
  virtual void Free(void *ptr) = 0;
};

} // namespace cxx