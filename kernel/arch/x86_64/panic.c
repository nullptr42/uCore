/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <lib/libc/stdio.h>
#include <lib/vt100/codes.h>

void panic() {
    puts(VT100_COLOR_FG_RED "KERNEL PANIC!");
    asm("cli;hlt");
}