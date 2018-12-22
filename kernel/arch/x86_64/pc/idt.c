
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

// Exception stubs
void isr_routine_0x00();
void isr_routine_0x01();
void isr_routine_0x02();
void isr_routine_0x03();
void isr_routine_0x04();
void isr_routine_0x05();
void isr_routine_0x06();
void isr_routine_0x07();
void isr_routine_0x08();
void isr_routine_0x09();
void isr_routine_0x0A();
void isr_routine_0x0B();
void isr_routine_0x0C();
void isr_routine_0x0D();
void isr_routine_0x0E();
void isr_routine_0x0F();
void isr_routine_0x10();
void isr_routine_0x11();
void isr_routine_0x12();
void isr_routine_0x13();
void isr_routine_0x14();
void isr_routine_0x15();
void isr_routine_0x16();
void isr_routine_0x17();
void isr_routine_0x18();
void isr_routine_0x19();
void isr_routine_0x1A();
void isr_routine_0x1B();
void isr_routine_0x1C();
void isr_routine_0x1D();
void isr_routine_0x1E();
void isr_routine_0x1F();

// IRQ stubs
void isr_routine_0x20();
void isr_routine_0x21();
void isr_routine_0x22();
void isr_routine_0x23();
void isr_routine_0x24();
void isr_routine_0x25();
void isr_routine_0x26();
void isr_routine_0x27();
void isr_routine_0x28();
void isr_routine_0x29();
void isr_routine_0x2A();
void isr_routine_0x2B();
void isr_routine_0x2C();
void isr_routine_0x2D();
void isr_routine_0x2E();
void isr_routine_0x2F();

// Syscall stub
void isr_routine_0x30();

void idt_init() {
    for (int i = 0; i < IDT_NUM_ENT; i++)
        idt_set_entry(&idt[i], NULL, 0, 0);
    
    idt_set_entry(&idt[0x00], isr_routine_0x00, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x01], isr_routine_0x01, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x02], isr_routine_0x02, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x03], isr_routine_0x03, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x04], isr_routine_0x04, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x05], isr_routine_0x05, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x06], isr_routine_0x06, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x07], isr_routine_0x07, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x08], isr_routine_0x08, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x09], isr_routine_0x09, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0A], isr_routine_0x0A, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0B], isr_routine_0x0B, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0C], isr_routine_0x0C, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0D], isr_routine_0x0D, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0E], isr_routine_0x0E, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0F], isr_routine_0x0F, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x10], isr_routine_0x10, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x11], isr_routine_0x11, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x12], isr_routine_0x12, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x13], isr_routine_0x13, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x14], isr_routine_0x14, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x15], isr_routine_0x15, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x16], isr_routine_0x16, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x17], isr_routine_0x17, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x18], isr_routine_0x18, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x19], isr_routine_0x19, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1A], isr_routine_0x1A, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1B], isr_routine_0x1B, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1C], isr_routine_0x1C, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1D], isr_routine_0x1D, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1E], isr_routine_0x1E, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1F], isr_routine_0x1F, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x20], isr_routine_0x20, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x21], isr_routine_0x21, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x22], isr_routine_0x22, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x23], isr_routine_0x23, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x24], isr_routine_0x24, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x25], isr_routine_0x25, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x26], isr_routine_0x26, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x27], isr_routine_0x27, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x28], isr_routine_0x28, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x29], isr_routine_0x29, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2A], isr_routine_0x2A, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2B], isr_routine_0x2B, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2C], isr_routine_0x2C, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2D], isr_routine_0x2D, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2E], isr_routine_0x2E, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2F], isr_routine_0x2F, SEG_KCODE_LM, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    
    idt_reload(&idt_ptr);
}