/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <kernel/version.hpp>

namespace kernel {

KernelInfo g_kernel_info = {.name = "reinix",
                            .version = {.major = 0, .minor = 1}};
}