/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/print.h>
#include <lib/vt100/vt100-codes.h>

void panic() {
    kprint(VT100_COLOR_FG_RED "KERNEL PANIC!");
    asm("cli;hlt");
}