/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <arch/print.h>
#include <arch/x86_64/pc/bootinfo.h>
#include <arch/x86_64/pc/vm/vm.h>

#include "stack.h"

extern const void kernel_start;
extern const void kernel_end;

void pm_init(struct bootinfo* binf) {
    uintptr_t minimum = (uintptr_t)&kernel_end - VM_BASE_KERNEL_ELF + 1;

    trace("pm: Initializing Physical Memory Manager.");
    trace("pm: Scanning Memory Map");

    /* Determine lowest safe memory address */
    for (int i = 0; i < binf->num_modules; i++) {
        struct module* module = &binf->modules[i];

        if ((uintptr_t)module->last > minimum)
            minimum = (uintptr_t)module->last + 1;
    }

    /* Align minimum address to next page boundary */
    if ((minimum % 4096) != 0)
        minimum += 4096 - (minimum % 4096);

    append("\t-> Lowest usable address is: %p", (void*)minimum);

    /* Initialize the page stack, the primary source for pages */
    if (!pm_init_stack(binf, (void*)minimum)) {
        error("pm: Unable to allocate physical page stack!");
        /* TODO: panic... */
        return;
    }

    kprint("\n");
}
