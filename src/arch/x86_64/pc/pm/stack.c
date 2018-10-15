/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <stddef.h>

#include "pm.h"
#include "stack.h"

/* Stack that stores free pages */
static struct stack {
    uint32_t* pages;
    int pos;
    int len;
} stack;

static void show_size(uint64_t size) {
    char* unit = "B";

    if (size >= (1024*1024*1024)) {
        unit = "GiB";
        size /= 1024*1024*1024;
    } else if (size >= (1024*1024)) {
        unit = "MiB";
        size /= 1024*1024;
    } else if (size >= 1024) {
        unit = "KiB";
        size /= 1024;
    }

    info("pm: total %d %s of memory found.", size, unit);    
}

bool pm_init_stack(struct bootinfo* binf, void* minimum) {
    uint64_t memory_size = 0;
    int num_mmap = binf->num_mmap;

    /* Count available memory */
    for (int i = 0; i < num_mmap; i++) {
        struct memory* memory = &binf->mmap[i];

        memory_size += (memory->last - memory->base) + 1;
    }

    /* Show size of available memory */
    show_size(memory_size);

    uint64_t num_pages = memory_size / 4096;

    if (memory_size % 4096) num_pages++; 

    uint64_t stack_size = sizeof(uint32_t) * num_pages;

    stack.pages = NULL;
    stack.len = num_pages;
    stack.pos = 0;

    /* Allocate memory area for the stack */
    for (int i = 0; i < num_mmap; i++) {
        struct memory* memory = &binf->mmap[i];
        uint64_t base = memory->base;
        uint64_t last = memory->last;

        /* Base must not be lower than minimum safe address */
        if (base < (uint64_t)minimum)
            base = (uint64_t)minimum;
        if (base >= last)
            continue;

        /* Try to allocate stack in this region */
        if ((last - base + 1) >= stack_size) {
            memory->base = base + stack_size;
            stack.pages = (void*)base;
            break;
        }
    }

    if (stack.pages == NULL)
        return false;
    info("pm: stack allocated @ %p", stack.pages);

    /* Release free pages into the stack */
    for (int i = 0; i < num_mmap; i++) {
        struct memory* memory = &binf->mmap[i];
        uint64_t base = memory->base;
        uint64_t last = memory->last;
        
        /* Base must not be lower than minimum safe address */
        if (base < (uint64_t)minimum)
            base = (uint64_t)minimum;
        if (base >= last)
            continue;

        uint32_t start = base / 4096;
        uint32_t end   = last / 4096;

        if ((start % 4096) != 0) start++;

        for (uint32_t j = start; j < end; j++)
            pm_stack_free(1, &j);
    }

    return true;
}

enum pmm_status pm_stack_alloc(int num, uint32_t* pages) {
    if (num > stack.len || num <= 0 || pages == NULL)
        return PMM_BAD_REQUEST;
    if (num > stack.pos)
        return PMM_OUT_OF_MEMORY;

    stack.pos -= num;
    for (int i = 0; i < num; i++)
        pages[i] = stack.pages[stack.pos + i];

    return PMM_OK;
}

enum pmm_status pm_stack_free(int num, uint32_t* pages) {
    if (num > stack.len || num <= 0 || pages == NULL ||
       (stack.pos + num) > stack.len)
        return PMM_BAD_REQUEST;

    for (int i = 0; i < num; i++)
        stack.pages[stack.pos + i] = pages[i];
    stack.pos += num;

    return PMM_OK;
}
