/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "state.hpp"

#include <lib/rxx/stdio.hpp>

using namespace arch::x86_64;

extern "C" CpuState *handle_interrupt(CpuState *cpu) {
  if (cpu->intr == 0x30) {
    rxx::printf("syscall 0x30!\n");
  }
  return cpu;
}
