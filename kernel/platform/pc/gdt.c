/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/x86_64/gdt.h>

#define NUM_ENTRIES (5)

#define GDT_ACCESS_RING0 (GDT_PRESENT | GDT_RING_0 | GDT_SEGMENT | GDT_RW)
#define GDT_ACCESS_RING3 (GDT_PRESENT | GDT_RING_3 | GDT_SEGMENT | GDT_RW)

struct gdt_entry gdt[NUM_ENTRIES];

static struct gdt_pointer gdt_ptr = {
    .limit   = NUM_ENTRIES * GDT_ENT_SIZE - 1,
    .pointer = &gdt[0]
};

void gdt_init() {
    // Null descriptor
    // Kernel 64-bit Code Segment (selector=0x08)
    // Kernel Data Segment (selector=0x10)
    // User 64-bit Code Segment (selector=0x1B)
    // User Data Segment (selector=0x23) 
    gdt_set_entry(&gdt[0], 0, 0, 0, 0);
    gdt_set_entry(&gdt[1], 0, 0xFFFFF, GDT_GRANULARITY | GDT_LM, GDT_ACCESS_RING0 | GDT_EXEC);
    gdt_set_entry(&gdt[2], 0, 0xFFFFF, GDT_GRANULARITY | GDT_PM_32, GDT_ACCESS_RING0);
    gdt_set_entry(&gdt[3], 0, 0xFFFFF, GDT_GRANULARITY | GDT_LM, GDT_ACCESS_RING3 | GDT_EXEC);
    gdt_set_entry(&gdt[4], 0, 0xFFFFF, GDT_GRANULARITY | GDT_PM_32, GDT_ACCESS_RING3);

    /* Tell CPU to load the new GDT */ 
    gdt_reload((void*)&gdt_ptr, 0x08, 0x10);
}
