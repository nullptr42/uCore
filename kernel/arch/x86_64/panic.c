/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/print.h>
#include <lib/vt100-codes.h>

void panic() {
    kprint(COLOR_RED "KERNEL PANIC!");
    asm("cli;hlt");
}