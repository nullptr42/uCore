/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_IDT_H_
#define _X86_64_IDT_H_

#include <stdint.h>

// Number of interrupt vectors.
#define IDT_NUM_ENT (256)

#define IDT_LIMIT (IDT_NUM_ENT * sizeof(struct idt_entry) - 1)

/**
 * @brief Interrupt descriptor flags
 */
enum idt_flags {
    // Gate Type
    IDT_GATE_TASK = 5,
    IDT_GATE_INTR = 6,
    IDT_GATE_TRAP = 7,

    // 64-bit or 32-bit entry. Always 1 in long mode.
    IDT_LM = 1<<3,    
    
    // Minimum Descriptor Privilege Level (DPL)
    IDT_RING_0 = 0<<5,
    IDT_RING_1 = 1<<5,
    IDT_RING_2 = 2<<5,
    IDT_RING_3 = 3<<5,

    IDT_ACTIVE = 1<<7
};

/**
 * @brief Structure required by LIDT, stores IDT address and size.
 */
struct idt_pointer {
    uint16_t limit;
    void* pointer;
} __attribute__((packed));

/**
 * @brief 64-bit packed IDT entry.
 */
struct idt_entry {
    uint16_t offset_lo;
    uint16_t segment;
    uint8_t  ist; /* interrupt stack table */
    uint8_t  flags;
    uint16_t offset_mi;
    uint32_t offset_hi;
    uint32_t padding;
} __attribute__((packed));

/**
 * Load Interrupt Descriptor Table.
 *
 * @param[in]  pointer  Contains IDT address and size.
 */
void idt_reload(struct idt_pointer* pointer);

/**
 * Pack interrupt information into IDT entry.
 *
 * @param[in]  entry        Table entry
 * @param      handler      Handler function pointer
 * @param      codesegment  Code Segment
 * @param      flags        Flags (e.g. minimum required DPL or gate type).
 */
static inline void idt_set_entry(struct idt_entry* entry,
                                 void* handler,
                                 int codesegment,
                                 int flags) {
    uint64_t offset = (uint64_t)handler;
    
    entry->offset_lo =  offset  & 0xFFFF;
    entry->offset_mi = (offset >> 16) & 0xFFFF;
    entry->offset_hi =  offset >> 32;
    entry->ist       = 0;
    entry->flags     = flags;
    entry->segment   = codesegment;
    entry->padding   = 0; 
}

#endif //_X86_64_IDT_H_
