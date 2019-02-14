/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <arch/print.h>
#include <stddef.h>
#include <arch/x86_64/vm/vm.h>
#include <arch/x86_64/pm/stack.h>

/*
 * Memory Map:
 * 0x0000000000000000 - 0xFFFF7FFFFFFFFFFF: Reserved for user processes.
 * 0xFFFF800000000000 - 0xFFFF807FFFFFFFFF: Recursive mapping
 * 0xFFFF808000000000 - 0xFFFF8080000FFFFF: First 1MiB of physical memory.
 * 0xFFFF808000100000 - 0xFFFFFFFEFFFFFFFF: Unmapped (maybe map rest of Physical Memory?)
 * 0xFFFFFFFF00000000 - 0xFFFFFFFF7FFFFFFF: Kernel Page Frames (2 GiB)
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

void vm_set_ctx(struct vm_context* ctx) {
    asm("mov %0, %%cr3\n" : : "r" (&ctx->pml4[0]));
}

void vm_init() {
    uint64_t* pml4_old = (void*)&vm_level1;
    uint64_t* pml4_new = &vm_kctx.pml4[0];

    struct pm_stack* stack = pm_get_stack();

    uint64_t pml4_phys = (ptentry_t)&pml4_new[0] - VM_BASE_KERNEL_ELF;

    pml4_active = build_address(256, 256, 256, 256);

    /* Setup recursive mapping to new PML4 in old and new PML4. 
     * This way we can use standard mapping functions to map the kernel 
     * to the new PML4 nicely and then switch to the new context. */
    pml4_old[256] = pml4_new[256] 
                  = PT_MAPPED | PT_WRITEABLE | pml4_phys;

    /* Map VGA buffer */
    vm_map_page((void*)0xFFFFFFFF800B8000, 0xB8000 / 4096);

    /* Map kernel */
    vm_map_block(
        (void*)&kernel_start,
        (void*)&kernel_start - VM_BASE_KERNEL_ELF, 
        (uint64_t)&kernel_end - (uint64_t)&kernel_start + 1
    );   

    /* Initialize the virtual page frame allocator */
    vm_alloc_init();

    int   stack_size = stack->len * sizeof(uint32_t);
    void* stack_phys = stack->pages;
    int   stack_cnt  = stack_size / 4096;

    if ((stack_size % 4096) != 0)
        stack_cnt++;

    void* stack_virt = vm_alloc(stack_cnt);

    /* Mapping physical page stack */
    klog(LL_DEBUG, "vm: Mapping page stack @ virtual %p.", stack_virt);
    vm_map_block(stack_virt, stack_phys, stack_size);

    /* Map first 1MiB of physical memory */
    vm_map_block((void*)0xFFFF808000000000, NULL, 1024*1024);

    /* Enable the new context */
    klog(LL_TRACE, "vm: Set new PML4 address (cr3)...");
    vm_set_ctx((void*)pml4_phys);

    /* Point physical page stack to its virtual address. */
    stack->pages = stack_virt;
}
