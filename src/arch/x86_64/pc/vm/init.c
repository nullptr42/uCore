/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <arch/print.h>
#include "vm.h"
#include "../pm/stack.h"

/*
 * Memory Map:
 * 0x0000000000000000 - 0xFFFF7FFFFFFFFFFF: Reserved for user processes.
 * 0xFFFF800000000000 - 0xFFFFFFFEFFFFFFFF: Unmapped (maybe map Physical Memory?)
 * 0xFFFFFFFF00000000 - 0xFFFFFFFF7FFFFFFF: Kernel Allocated Memory (2 GiB)
 * 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF: Kernel Executable (2 GiB)
 */

/* Virtual address of active PML4 table (via recursive mapping) */
ptentry_t* pml4_active;

struct vm_context vm_kctx __pgalign;

extern const void kernel_start;
extern const void kernel_end;
extern const void vm_level1;

static inline void* build_address(int pml4, int pdpt, int pd, int pt) {
    return (void*)(
        ((uint64_t)pml4 << 39) |
        ((uint64_t)pdpt << 30) |
        ((uint64_t)pd   << 21) |
        ((uint64_t)pt   << 12) |
        0xFFFF000000000000ULL
    );
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

    /* Initialize the virtual page frame allocator */
    vm_alloc_init();

    /* Mapping the physical page stack */
    struct pm_stack* stack = pm_get_stack();
    int stack_size = stack->len * sizeof(uint32_t);
    void* stack_phys = stack->pages;
    void* stack_virt = vm_alloc(stack_size / 4096);

    append("\t-> Mapping physical page stack @ %p...", stack_virt);
    vm_map_block(stack_virt, stack_phys, stack_size);

    /* Enable the new context */
    append("\t-> Enable new paging context (set cr3)...");
    asm("mov %0, %%cr3\n" : : "r" (pml4_phys));

    /* Point physical page stack to its virtual address. */
    stack->pages = stack_virt;

    /* Throw a party if everything worked out! */
    trace("vm: Survived!");
    kprint("\n");
}
