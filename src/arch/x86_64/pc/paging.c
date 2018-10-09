/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>

#define PT_MAPPED         (1<<0)
#define PT_WRITEABLE      (1<<1)
#define PT_USERSPACE      (1<<2)
#define PT_WRITE_THROUGH  (1<<3)
#define PT_DISABLE_CACHE  (1<<4)
#define PT_WAS_USED       (1<<5)
#define PT_HUGE_PAGES     (1<<7)
#define PT_NO_EXECUTE     (1<<63)

#define __pgalign __attribute__((aligned(0x1000)))

/*
 * Memory Map:
 * 0x0000000000000000 - 0xFFFF7FFFFFFFFFFF: Reserved for user processes.
 * 0xFFFF800000000000 - 0xFFFFFFFEFFFFFFFF: Physical Memory
 * 0xFFFFFFFF00000000 - 0xFFFFFFFF7FFFFFFF: Kernel Allocated Memory
 * 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF: Kernel Executable
 */

#define KERNEL_VBASE (0xFFFFFFFF80000000)

#define PML4_LO_ENT (256)
#define PML4_HI_ENT (256)

#define PDPT_PHYS_ENT (508)
#define PDPT_KRNL_ENT (4)

/* Represents a single page table entry */
typedef uint64_t ptentry_t;

/* Represents a single address space */
struct pg_context {
    ptentry_t pml4[512];
};

static struct pg_context pg_kctx __pgalign;

void pg_init_ctx(struct pg_context* ctx) {
    for (int i = 0; i < PML4_LO_ENT; i++)
        ctx->pml4[i] = 0;
    for (int i = PML4_LO_ENT; i < PML4_HI_ENT; i++)
        ctx->pml4[i] = pg_kctx.pml4[i];
}

void pg_init() {
    for (int i = 0; i < PML4_LO_ENT; i++)
        pg_kctx.pml4[i] = 0;

    for (int i = PML4_LO_ENT; i < 512; i++) {
        //pg_kctx.pml4[i] = (ptentry_t)(&pg_kpdpt[i - PML4_LO_ENT]) |
        //                  PT_MAPPED |
        //                  PT_WRITEABLE;
    }

    //asm("mov %0, %%cr3" : : "r" ((void*)&pg_kctx.pml4 - KERNEL_VBASE));
}
