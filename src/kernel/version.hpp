/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

namespace kernel {

struct KernelInfo {
  const char *name;
  struct VersionTag {
    int major;
    int minor;
  } version;
};

extern KernelInfo g_kernel_info;

} // namespace kernel