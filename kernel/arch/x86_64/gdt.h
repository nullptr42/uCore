/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_GDT_H_
#define _X86_64_GDT_H_

#include <stdint.h>

// Flags
#define GDT_GRANULARITY (1<<3)
#define GDT_PM_32       (1<<2)
#define GDT_LM          (1<<1)
#define GDT_PROGRAMMER  (1<<0)

// Access Byte
#define GDT_PRESENT  (1<<7)
#define GDT_RING_0   (0<<5)
#define GDT_RING_1   (1<<5)
#define GDT_RING_2   (2<<5)
#define GDT_RING_3   (3<<5)
#define GDT_SEGMENT  (1<<4)
#define GDT_EXEC     (1<<3)
#define GDT_DC_BIT   (1<<2)
#define GDT_RW       (1<<1)
#define GDT_ACCESSED (1<<0)
#define GDT_TSS_386  (9)

/**
 * @brief GDT pointer required by LGDT.
 */
struct gdt_pointer {
    uint16_t limit;
    void*    pointer;
} __attribute__((packed));

/**
 * @brief Packed GDT entry containing base, limit, access and misc. flags.
 */
struct gdt_entry {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t  base_mi;
    uint8_t  access;
    uint8_t  limit_hi : 4;
    uint8_t  flags    : 4;
    uint8_t  base_hi;
} __attribute__((packed));

#define GDT_ENT_SIZE (sizeof(struct gdt_entry))

/**
 * Set new Global Descriptor Table.
 * 
 * @param[in]  pointer      GDT pointer
 * @param[in]  codesegment  New code segment
 * @param[in]  datasegment  New data segment  
 */
void gdt_reload(struct gdt_pointer* pointer, uint8_t codesegment, uint8_t datasegment);

/**
 * Pack GDT entry information into 8-byte structure.
 *
 * @param[in]  entry   GDT entry
 * @param[in]  base    Memory base address
 * @param[in]  limit   Memory size
 * @param[in]  flags   Flags such as Long-Mode / Protected-Mode
 * @param[in]  access  Access flags (e.g. privilege level)
 */
static inline void gdt_set_entry(struct gdt_entry* entry,
                                 uint32_t base,
                                 uint32_t limit,
                                 int flags,
                                 int access) {
    // Set region base (32 bits)
    entry->base_lo =  base & 0xFFFF;
    entry->base_mi = (base >> 16) & 0xFF;
    entry->base_hi =  base >> 24;        

    // Set region size (20 bits)
    entry->limit_lo =  limit & 0xFFFF;
    entry->limit_hi = (limit >> 16) & 0xF;

    // Set access byte and flags (12 bits)
    entry->access = access & 0xFF;
    entry->flags  = flags  & 0xF;
}

#endif // _X86_64_GDT_H_