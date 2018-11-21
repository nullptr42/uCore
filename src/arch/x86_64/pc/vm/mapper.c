/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <stddef.h>
#include "../pm/stack.h"
#include "vm.h"

void panic();

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
            klog(LL_ERROR, "vm: get_or_create_table: Unable to allocate table.");
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
