/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>
#include <log.h>

#define PT_MAPPED         (1<<0)
#define PT_WRITEABLE      (1<<1)
#define PT_USERSPACE      (1<<2)
#define PT_WRITE_THROUGH  (1<<3)
#define PT_DISABLE_CACHE  (1<<4)
#define PT_WAS_USED       (1<<5)
#define PT_HUGE_PAGES     (1<<7)
#define PT_NO_EXECUTE     (1LL<<63)

#define __pgalign __attribute__((aligned(0x1000)))

#define KERNEL_VBASE (0xFFFFFFFF80000000)

typedef uint64_t ptentry_t;
ptentry_t pml4[512] __pgalign;

extern void vm_level2_b;

void vm_init() {
    void* _pdpt_b = (void*)&vm_level2_b - KERNEL_VBASE;
    void* _pml4 = (void*)&pml4[0] - KERNEL_VBASE;
    
    trace("vm: Initializing Virtual Memory Mananger.");
    append("\t-> Setup initial PML4.");
    
    /* Clear all entries */
    for (int i = 0; i < 512; i++)
        pml4[i] = 0;
    pml4[511] = PT_MAPPED | PT_WRITEABLE | (ptentry_t)_pdpt_b;

    append("\t-> Setup recursive mapping...");
    pml4[256] = PT_MAPPED | PT_WRITEABLE | (ptentry_t)_pml4;

    /* Enable the new context */
    append("\t-> Enable new paging context (set cr0)...");
    asm("mov %0, %%cr3\n" : : "r" (_pml4));

    /* Throw a party if everything worked out! */
    trace("vm: Survived!");

    //uint64_t* test = (void*)((256LL << 39) | (256LL << 30) | (256LL << 21) | (256LL << 12) | 0xFFFF000000000000ULL);
    //trace("vm: test recursive mapping 0x%08X equals 0x%08X?", pml4[511], test[511]);
    kprint("\n");
}





/*
 * Memory Map:
 * 0x0000000000000000 - 0xFFFF7FFFFFFFFFFF: Reserved for user processes.
 * 0xFFFF800000000000 - 0xFFFFFFFEFFFFFFFF: Physical Memory
 * 0xFFFFFFFF00000000 - 0xFFFFFFFF7FFFFFFF: Kernel Allocated Memory
 * 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF: Kernel Executable
 */
