/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "vm.h"
#include <log.h>
#include <stddef.h>

/* Kernel can allocate up to 2GiB of virtual address space
 * which will be located in the region:
 * 0xFFFFFFFF00000000 - 0xFFFFFFFF7FFFFFFF
 */

#define ALLOC_BASE  (0xFFFFFFFF00000000LL)
#define ALLOC_LIMIT (0xFFFFFFFF7FFFFFFFLL)

/* that amounts to 2GiB of allocatable address space */
#define NUM_ENTRIES (8192)
#define PAGES_PER_ENTRY (64)


static int hint = 0;
static uint64_t bitmap[NUM_ENTRIES];

static const uint64_t qword_free = 0xFFFFFFFFFFFFFFFFLL;

void panic();

void vm_alloc_init() {
    /* Reset bitmap */
    for (int i = 0; i < NUM_ENTRIES; i++)
        bitmap[i] = qword_free;
}

static int vm_alloc_small(int count) {
    for (int entry = hint; entry < NUM_ENTRIES; entry++) {
        if (bitmap[entry] == 0)
            continue;

        /* If all pages are free just take the first "n" 
         * and mark them as allocated.
         */
        if (bitmap[entry] == qword_free) {
            hint = entry + 1;
            bitmap[entry] &= qword_free << count;
            return (entry * PAGES_PER_ENTRY); 
        }

        uint64_t tmp  = bitmap[entry];
        uint64_t mask = ~(qword_free << count);

        /* Otherwise search for string of "n" free pages. */
        for (int i = 0; i <= (PAGES_PER_ENTRY - count); i++) {
            if ((tmp & mask) == mask) {
                bitmap[entry] &= ~mask;
                hint = entry + 1;
                return (entry * PAGES_PER_ENTRY) + i;
            }
            mask <<= 1;
        }
    }

    return -1;
}

static int vm_alloc_large(int count) {
    for (int entry = hint; entry < NUM_ENTRIES; entry++) {
        /* Bail out quickly if no pages are free in this entry */
        if (bitmap[entry] == 0)
            continue;

        /* For performance reasons we only take entries that are completely free */
        if (bitmap[entry] == qword_free) {
            int entry2 = entry + 1;
            int status = 0;
            int remain = count - PAGES_PER_ENTRY;

            /* Scan following entries for free pages until
             * the desired amount is reached.
             */
            while (remain > 0) {
                if (entry2 >= NUM_ENTRIES || bitmap[entry2] != qword_free) {
                    status = -1;
                    break;
                }
                remain -= PAGES_PER_ENTRY;
                entry2++;;
            }

            /* TODO: use evil gotos instead. */
            if (status == -1) {
                continue;
            }

            /* Mark pages as allocated. */
            if (remain < 0) {
                for (int i = entry; i < entry2 - 1; i++)
                    bitmap[i] = 0;
                bitmap[entry2-1] &= (qword_free << (remain * -1));
            } else {
                for (int i = entry; i < entry2; i++)
                    bitmap[i] = 0;
            }

            hint = entry2;
                        
            return (entry * PAGES_PER_ENTRY);
        }
    }

    return -1;
}

void* vm_alloc(int count) {
    /* Catch invalid requests */
    if (count <= 0)
        return NULL;

    int start;
    hint = 0;
retry:
    /* Depening on the number of pages requested
     * we can either use a slow or fast algorithm.
     */
    if (count <= PAGES_PER_ENTRY) {
        start = vm_alloc_small(count);
    } else {
        start = vm_alloc_large(count);
    }

    /* If allocation failed but we did not start from
     * the beginning of the bitmap give it another try
     */
    if (start == -1) {
        if (hint != 0) {
            hint = 0;
            goto retry;
        }
        klog(LL_ERROR, "vm: vm_alloc: cannot satisfy allocation of %d page(s).", count);
        panic();
        return NULL;
    }

    return (void*)((uint64_t)start * 4096) + ALLOC_BASE;
}

void vm_free(void* virtual, int count) {
    /* Ensure virtual address is within legal bounds. */
    if ((uint64_t)virtual < ALLOC_BASE ||
        (uint64_t)virtual > ALLOC_LIMIT) {
        klog(LL_ERROR, "vm: vm_free: address %p is not within bounds.", virtual);
        panic();
        return;
    }

    virtual -= ALLOC_BASE;

    int page = (int)((uint64_t)virtual / 4096);

    /* TODO: this is kind of cheap. Optimize later. */
    for (int i = page; i < (page + count); i++) {
        int entry = i / PAGES_PER_ENTRY;
        int index = i % PAGES_PER_ENTRY;

        bitmap[entry] |= ~(1 << index);
    }

    hint = page / PAGES_PER_ENTRY;
} 