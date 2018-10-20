/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "vm.h"
#include <log.h>

/* Kernel can allocate at max. 2GiB of virtual memory */
//#define MAX_KERNEL_MEMORY (2LL*1024*1024*1024)
//#define NUM_ENTRIES (MAX_KERNEL_MEMORY / 4096 / 8 / sizeof(uint64_t))

/* that amounts to 2GiB of allocatable address space */
#define NUM_ENTRIES (8192)

enum page_state {
    PG_FREE = 0,
    PG_USED = 1
};

static uint64_t bitmap[NUM_ENTRIES];

void vm_alloc_init() {
    /* Clear bitmap */
    for (int i = 0; i < NUM_ENTRIES; i++)
        bitmap[i] = 0;
}

//void 