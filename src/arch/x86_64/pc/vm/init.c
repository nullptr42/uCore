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

static inline void* build_address(int pml4, int pdpt, int pd, int pt) {
    return (void*)(
        ((uint64_t)pml4 << 39) |
        ((uint64_t)pdpt << 30) |
        ((uint64_t)pd   << 21) |
        ((uint64_t)pt   << 12) |
        0xFFFF000000000000ULL
    );
}

static inline void* pml4_address() {
    return build_address(256, 256, 256, 256);
}

static inline void* pdpt_address(int pdpt) {
    return build_address(256, 256, 256, pdpt);
}

static inline void* pd_address(int pdpt, int pd) {
    return build_address(256, 256, pdpt, pd);
}

static inline void* pt_address(int pdpt, int pd, int pt) {
    return build_address(256, pdpt, pd, pt);
}

void panic();

void vm_init() {
    void* _pdpt_b = (void*)&vm_level2_b - KERNEL_VBASE;
    void* _pml4 = (void*)&pml4[0] - KERNEL_VBASE;
    uint64_t* pml4r = pml4_address(); /* recursive mapped PML4 table */ 
    
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

    /* Recursive map self-test */
    for (int i = 0; i < 512; i++) {
        if (pml4r[i] != pml4[i]) {
            error("vm: Recursive map self-test failed PML4-index=%d", i);
            panic();
        }
    }
    append("\t-> Recursive map self-test completed.");

    /* Throw a party if everything worked out! */
    trace("vm: Survived!");
    kprint("\n");
}





/*
 * Memory Map:
 * 0x0000000000000000 - 0xFFFF7FFFFFFFFFFF: Reserved for user processes.
 * 0xFFFF800000000000 - 0xFFFFFFFEFFFFFFFF: Physical Memory
 * 0xFFFFFFFF00000000 - 0xFFFFFFFF7FFFFFFF: Kernel Allocated Memory
 * 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF: Kernel Executable
 */
