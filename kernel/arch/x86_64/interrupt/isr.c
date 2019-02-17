/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/x86_64/state.h>
#include <lib/libc/stdio.h>

struct cpu_state* handle_interrupt(struct cpu_state* cpu) {
    if (cpu->intr >= 0x20 && cpu->intr <= 0x2F) {
        printf("hw irq: %x\n", cpu->intr);
        pic_send_eoi(cpu->intr - 0x20);
    }
    return cpu;
}
