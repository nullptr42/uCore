/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <reinix/types.h>

namespace kernel {

class HeapBreak {

public:
  virtual void *Grow(size_t size) = 0;
  virtual void *Shrink(size_t size) = 0;
};

} // namespace kernel