/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>
#include <arch/x86_64/idt.h>

#include "gdt.h"

static struct idt_entry idt[IDT_NUM_ENT];

static struct idt_pointer idt_ptr = {
    .limit   = IDT_LIMIT,
    .pointer = &idt[0]
};

extern void isr_test();

void idt_init() {
    for (int i = 0; i < IDT_NUM_ENT; i++) {
        idt_set_entry(&idt[i], NULL, 0, 0);
    }
    for (int i = 0; i < 0x20; i++) {
        idt_set_entry(&idt[i], isr_test, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    }
    idt_reload(&idt_ptr);
}