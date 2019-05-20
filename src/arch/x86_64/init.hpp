/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <kernel/bootinfo.hpp>

namespace arch::x86_64 {

void initialize(kernel::BootInfo *bootinfo);
}