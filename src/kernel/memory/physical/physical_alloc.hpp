/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <reinix/types.h>

namespace kernel {

struct FrameAllocator {
  enum Flags { None = 0, Linear = (1 << 0) };

  enum class Status { BadRequest, OutOfMemory, Success };

  virtual Status Alloc(size_t count, uint64_t *pages, int flags) = 0;
  virtual Status Free(size_t count, uint64_t *pages) = 0;

  // Status Alloc(rxx::Array<page_t>& )

  /*enum class Status {

  };

  virtual Status Alloc(size_t count, uint64_t* pages) = 0;
  virtual Status Free(size_t count, uint64_t* pages) = 0;*/
};

} // namespace kernel