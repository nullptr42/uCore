/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/x86_64/gdt.hpp>
#include <stddef.h>
#include <stdint.h>

using namespace arch::x86_64;

namespace arch::x86_64::idt {

enum GateType { Task = 5, Interrupt = 6, Trap = 7 };

typedef void (*IrqHandler)();

struct IdtEntry {
  uint16_t offset_lo = 0;
  uint16_t segment = 0;
  uint8_t int_stack_tbl = 0;
  uint8_t flags = 0;
  uint16_t offset_mi = 0;
  uint32_t offset_hi = 0;
  uint32_t padding = 0;

  IdtEntry() {}

  IdtEntry(IrqHandler handler, bool active, bool longmode, int ring,
           uint16_t codesegment, GateType type) {
    offset_lo = (uint64_t)handler & 0xFFFF;
    offset_mi = ((uint64_t)handler >> 16) & 0xFFFF;
    offset_hi = (uint64_t)handler >> 32;
    flags = (active ? 0x80 : 0) | (longmode ? 0x8 : 0) | ((ring & 3) << 5) |
            int(type);
    segment = codesegment;
  }
} __attribute__((packed));

struct IdtPointer {
  uint16_t limit;
  IdtEntry *pointer;
} __attribute__((packed));

static IdtEntry g_idt_table[256];

static IdtPointer g_idt_pointer = {.limit = sizeof(g_idt_table) - 1,
                                   .pointer = &g_idt_table[0]};

/* Exception stubs */
extern "C" void isr_routine_0x00();
extern "C" void isr_routine_0x01();
extern "C" void isr_routine_0x02();
extern "C" void isr_routine_0x03();
extern "C" void isr_routine_0x04();
extern "C" void isr_routine_0x05();
extern "C" void isr_routine_0x06();
extern "C" void isr_routine_0x07();
extern "C" void isr_routine_0x08();
extern "C" void isr_routine_0x09();
extern "C" void isr_routine_0x0A();
extern "C" void isr_routine_0x0B();
extern "C" void isr_routine_0x0C();
extern "C" void isr_routine_0x0D();
extern "C" void isr_routine_0x0E();
extern "C" void isr_routine_0x0F();
extern "C" void isr_routine_0x10();
extern "C" void isr_routine_0x11();
extern "C" void isr_routine_0x12();
extern "C" void isr_routine_0x13();
extern "C" void isr_routine_0x14();
extern "C" void isr_routine_0x15();
extern "C" void isr_routine_0x16();
extern "C" void isr_routine_0x17();
extern "C" void isr_routine_0x18();
extern "C" void isr_routine_0x19();
extern "C" void isr_routine_0x1A();
extern "C" void isr_routine_0x1B();
extern "C" void isr_routine_0x1C();
extern "C" void isr_routine_0x1D();
extern "C" void isr_routine_0x1E();
extern "C" void isr_routine_0x1F();

/* HW IRQ stubs */
extern "C" void isr_routine_0x20();
extern "C" void isr_routine_0x21();
extern "C" void isr_routine_0x22();
extern "C" void isr_routine_0x23();
extern "C" void isr_routine_0x24();
extern "C" void isr_routine_0x25();
extern "C" void isr_routine_0x26();
extern "C" void isr_routine_0x27();
extern "C" void isr_routine_0x28();
extern "C" void isr_routine_0x29();
extern "C" void isr_routine_0x2A();
extern "C" void isr_routine_0x2B();
extern "C" void isr_routine_0x2C();
extern "C" void isr_routine_0x2D();
extern "C" void isr_routine_0x2E();
extern "C" void isr_routine_0x2F();

/* Syscall stub */
extern "C" void isr_routine_0x30();

extern "C" void idt_reload(IdtPointer *pointer);

void initialize() {
  g_idt_table[0x00] = {isr_routine_0x00,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x01] = {isr_routine_0x01,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x02] = {isr_routine_0x02,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x03] = {isr_routine_0x03,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x04] = {isr_routine_0x04,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x05] = {isr_routine_0x05,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x06] = {isr_routine_0x06,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x07] = {isr_routine_0x07,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x08] = {isr_routine_0x08,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x09] = {isr_routine_0x09,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x0A] = {isr_routine_0x0A,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x0B] = {isr_routine_0x0B,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x0C] = {isr_routine_0x0C,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x0D] = {isr_routine_0x0D,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x0E] = {isr_routine_0x0E,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x0F] = {isr_routine_0x0F,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x10] = {isr_routine_0x10,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x11] = {isr_routine_0x11,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x12] = {isr_routine_0x12,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x13] = {isr_routine_0x13,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x14] = {isr_routine_0x14,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x15] = {isr_routine_0x15,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x16] = {isr_routine_0x16,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x17] = {isr_routine_0x17,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x18] = {isr_routine_0x18,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x19] = {isr_routine_0x19,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x1A] = {isr_routine_0x1A,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x1B] = {isr_routine_0x1B,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x1C] = {isr_routine_0x1C,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x1D] = {isr_routine_0x1D,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x1E] = {isr_routine_0x1E,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x1F] = {isr_routine_0x1F,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x20] = {isr_routine_0x20,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x21] = {isr_routine_0x21,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x22] = {isr_routine_0x22,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x23] = {isr_routine_0x23,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x24] = {isr_routine_0x24,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x25] = {isr_routine_0x25,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x26] = {isr_routine_0x26,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x27] = {isr_routine_0x27,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x28] = {isr_routine_0x28,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x29] = {isr_routine_0x29,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x2A] = {isr_routine_0x2A,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x2B] = {isr_routine_0x2B,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x2C] = {isr_routine_0x2C,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x2D] = {isr_routine_0x2D,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x2E] = {isr_routine_0x2E,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};
  g_idt_table[0x2F] = {isr_routine_0x2F,   true, true, 0, gdt::kKernelCodeSeg,
                       GateType::Interrupt};

  idt_reload(&g_idt_pointer);
}

} // namespace arch::x86_64::idt