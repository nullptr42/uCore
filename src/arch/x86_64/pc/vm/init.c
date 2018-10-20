/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <stdint.h>
#include <stddef.h>
#include "../pm/stack.h"
#include <arch/print.h>

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

struct vm_context {
    ptentry_t pml4[512];
};

/* Virtual address of active PML4 table (via recursive mapping) */
static ptentry_t* pml4_active;

struct vm_context vm_kctx __pgalign;

extern const void kernel_start;
extern const void kernel_end;
extern const void vm_level1;

void panic();

static inline void* build_address(int pml4, int pdpt, int pd, int pt) {
    return (void*)(
        ((uint64_t)pml4 << 39) |
        ((uint64_t)pdpt << 30) |
        ((uint64_t)pd   << 21) |
        ((uint64_t)pt   << 12) |
        0xFFFF000000000000ULL
    );
}

static inline void* get_child_address(ptentry_t* parent, int index) {
    return (void*)(((uint64_t)parent << 9) |
                   (index << 12) | 
                   0xFFFF000000000000ULL
                  );
}

static ptentry_t* get_or_create_table(ptentry_t* parent, int index) {
    ptentry_t* child = get_child_address(parent, index);
    
    /* Create table if it doesn't exist yet.*/
    if (~parent[index] & PT_MAPPED) {
        uint32_t new_page;
        
        /* Allocate a physical page to store the new table. */
        if (pm_stack_alloc(1, &new_page) != PMM_OK) {
            error("vm: unable to allocate a physical page.");
            panic();
        }

        /* Map table in parent table and clear its contents. */        
        parent[index] = PT_MAPPED | PT_WRITEABLE | ((ptentry_t)new_page * 4096);
        for (int i = 0; i < 512; i++)
            child[i] = 0;
    }

    return child;
}

/* TODO: check for unaligned input addresses */

void vm_map_page(void* virtual, uint32_t page) {
    int lvl4_idx = ((uint64_t)virtual >> 12) & 0x1FF;
    int lvl3_idx = ((uint64_t)virtual >> 21) & 0x1FF;
    int lvl2_idx = ((uint64_t)virtual >> 30) & 0x1FF;
    int lvl1_idx = ((uint64_t)virtual >> 39) & 0x1FF;

    ptentry_t* pml4 = pml4_active;
    ptentry_t* pdpt = get_or_create_table(pml4, lvl1_idx);
    ptentry_t* pd = get_or_create_table(pdpt, lvl2_idx);
    ptentry_t* pt = get_or_create_table(pd, lvl3_idx);
    
    pt[lvl4_idx] = PT_MAPPED | PT_WRITEABLE | ((ptentry_t)page * 4096);
}

void vm_map_pages(void* virtual, uint32_t* pages, int num) {
    /* TODO: optimize me */
    for (int i = 0; i < num; i++) {
        vm_map_page(virtual, pages[i]);
        virtual += 4096;
    }
}

void vm_map_block(void* virtual, void* physical, uint64_t size) {
    /* TODO: optimize me */
    int num = size / 4096;
    uint32_t start = (uint32_t)((uint64_t)physical / 4096);
    
    if ((size % 4096) != 0) num++;

    for (int i = 0; i < num; i++) {
        vm_map_page(virtual, start + i);
        virtual += 4096;
    }    
}

void vm_init() {
    uint64_t* pml4_old = (void*)&vm_level1;
    uint64_t* pml4_new = &vm_kctx.pml4[0];

    trace("vm: Initializing Virtual Memory Mananger.");

    uint64_t pml4_phys = (ptentry_t)&pml4_new[0] - KERNEL_VBASE;

    pml4_active = build_address(256, 256, 256, 256);

    /* Setup recursive mapping to new PML4 in old and new PML4. 
     * This way we can use standard mapping functions to map the kernel 
     * to the new PML4 nicely and then switch to the new context. */
    append("\t-> Setup recursive mapping...");
    pml4_old[256] = pml4_new[256] 
                  = PT_MAPPED | PT_WRITEABLE | pml4_phys;

    /* Map VGA buffer */
    append("\t-> Mapping VGA buffer...");
    vm_map_page((void*)0xFFFFFFFF800B8000, 0xB8000 / 4096);

    /* Map kernel */
    append("\t-> Mapping kernel executable...");
    vm_map_block(
        (void*)&kernel_start,
        (void*)&kernel_start - KERNEL_VBASE, 
        (uint64_t)&kernel_end - (uint64_t)&kernel_start + 1
    );   

    /* Enable the new context */
    append("\t-> Enable new paging context (set cr3)...");
    asm("mov %0, %%cr3\n" : : "r" (pml4_phys));

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
