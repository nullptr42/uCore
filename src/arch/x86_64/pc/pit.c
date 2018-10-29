/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>
#include <stdbool.h>
#include <arch/x86_64/io-port.h>

#define PIT_CMD_REG (0x43)
#define PIT_DATA_BASE (0x40)

enum pit_mode {
    PIT_MODE_0 = 0,
    PIT_MODE_1 = 1,
    PIT_MODE_2 = 2,
    PIT_MODE_3 = 3,
    PIT_MODE_4 = 4,
    PIT_MODE_5 = 5
};

void delay(int ms) {
    /* TODO: do sanity checks... */
    uint16_t frequency = 1193182 / 1000 * ms;

    /* Program Channel 2 for software one-shot. */
    outb(PIT_CMD_REG, (PIT_MODE_0<<1) | (3<<4) | (2<<6));
    outb(PIT_DATA_BASE+2, frequency & 0xFF);
    outb(PIT_DATA_BASE+2, frequency >> 8);

    while (~inb(0x61) & (1<<5)) { }
}
