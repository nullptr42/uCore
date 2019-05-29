/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include "heap_break.hpp"

namespace kernel {

class HeapAllocator {

  HeapAllocator(HeapBreak &brake) : brake(brake) {}

private:
  HeapBreak &brake;
};

} // namespace kernel