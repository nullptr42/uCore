/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/print.h>
#include <arch/x86_64/pc/state.h>

struct cpu_state* handle_interrupt(struct cpu_state* cpu) {
    kprint("Hello World!");
    return cpu;
}