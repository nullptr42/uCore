/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <arch/print.h>
#include <arch/x86_64/pc/bootinfo.h>

#include "stack.h"

#define KERNEL_VBASE (0xFFFFFFFF80000000)

extern const void kernel_start;
extern const void kernel_end;

void pm_init(struct bootinfo* binf) {
    uintptr_t minimum = (uintptr_t)&kernel_end - KERNEL_VBASE + 1;
    
    info("pm: initializing physical memory manager...");

    /* Determine lowest safe memory address */    
    for (int i = 0; i < binf->num_modules; i++) {
        struct module* module = &binf->modules[i];
        
        if ((uintptr_t)module->last > minimum)
            minimum = (uintptr_t)module->last + 1;
    }

    /* Align minimum address to next page boundary */
    if ((minimum % 4096) != 0)
        minimum += 4096 - (minimum % 4096); 

    info("pm: minimum address is %p", (void*)minimum);

    /* Initialize the page stack, the primary source for pages */
    if (!pm_init_stack(binf, (void*)minimum)) {
        error("pm: unable to allocate page stack.");
        /* TODO: panic... */
        return;
    }

    kprint("\n");
}

