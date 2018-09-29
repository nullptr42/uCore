/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

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

struct gdt_pointer {
    uint16_t limit;
    void*    pointer;
} __attribute__((packed));

struct gdt_entry {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t  base_mi;
    uint8_t  access;
    uint8_t  limit_hi : 4;
    uint8_t  flags    : 4;
    uint8_t  base_hi;
} __attribute__((packed));

#define NUM_ENTRIES (5)

struct gdt_entry gdt[NUM_ENTRIES];
static struct gdt_pointer gdt_ptr;

static inline void set_entry(struct gdt_entry* entry, uint32_t base, uint32_t limit, int flags, int access) {
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

extern void gdt_reload(struct gdt_pointer* ptr);

#define GDT_ACCESS_RING0 (GDT_PRESENT | GDT_RING_0 | GDT_SEGMENT | GDT_RW)
#define GDT_ACCESS_RING3 (GDT_PRESENT | GDT_RING_3 | GDT_SEGMENT | GDT_RW)

void gdt_init() {
    // Setup GDT pointer for LGDT-instruction.
    gdt_ptr.limit   = NUM_ENTRIES*8-1;
    gdt_ptr.pointer = &gdt[0];

    // Null descriptor
    set_entry(&gdt[0], 0, 0, 0, 0);

    // Kernel 64-bit Code Segment (selector=0x08)
    set_entry(&gdt[1], 0, 0xFFFFF, GDT_GRANULARITY | GDT_LM, GDT_ACCESS_RING0 | GDT_EXEC);

    // Kernel Data Segment (selector=0x10)
    set_entry(&gdt[2], 0, 0xFFFFF, GDT_GRANULARITY | GDT_PM_32, GDT_ACCESS_RING0);

    // User 64-bit Code Segment (selector=0x1B)
    set_entry(&gdt[3], 0, 0xFFFFF, GDT_GRANULARITY | GDT_LM, GDT_ACCESS_RING3 | GDT_EXEC);

    // User Data Segment (selector=0x23)
    set_entry(&gdt[4], 0, 0xFFFFF, GDT_GRANULARITY | GDT_PM_32, GDT_ACCESS_RING3);
    
    gdt_reload(&gdt_ptr);
}
