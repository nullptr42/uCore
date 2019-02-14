/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <stddef.h>
#include "pm/stack.h"
#include "vm/vm.h"

void panic();

void* libslab_get(size_t size) {
    int num = size / 4096;
    uint32_t pages[num];

    if (pm_stack_alloc(num, &pages[0]) != PMM_OK) {
        klog(LL_ERROR, "libslab_get: failed to allocate %d physical page(s).", num);
        panic();
    }

    void* base = vm_alloc(num);

    if (base == NULL) {
        klog(LL_ERROR, "libslab_get: failed to allocate %d virtual page(s).", num);
        panic();
    }

    vm_map_pages(base, pages, num);
    
    return base;
 }