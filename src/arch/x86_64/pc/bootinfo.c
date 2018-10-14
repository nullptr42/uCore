/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "bootinfo.h"

static bool callback(struct mb2_tag* tag, struct bootinfo* binf) {
    switch (tag->type) {
        case MB_TAG_MEMORY:
            binf->memory_lo = ((struct mb2_memory_tag*)tag)->mem_lower * 4096;
            binf->memory_hi = ((struct mb2_memory_tag*)tag)->mem_upper * 4096;
            return true;
        case MB_TAG_MMAP: {
            struct mb2_mmap_tag* mmap  = (void*)tag;

            uint32_t size = (uint32_t)sizeof(struct mb2_mmap_tag);
            struct mb2_mmap_ent* entry = (void*)mmap + size;
            
            while (size < tag->size) {
                if (entry->type == MB_MMAP_AVAILABLE) {
                    /* TODO: prevent buffer overrun... */
                    struct memory* memory = &binf->mmap[binf->num_mmap++];

                    memory->base = entry->base;
                    memory->last = entry->base + entry->length - 1;
                }
                
                /* update size and entry address */
                size += mmap->entry_size;
                entry = (void*)entry + mmap->entry_size;
            }
            return true;
        }
    }
    return false;
}

bool bootinfo_from_mb2(struct bootinfo* binf, struct mb2_info* mb2) {
    /* initialize basic data */
    binf->num_modules = 0;
    binf->num_mmap = 0;

    /* retrieve memory information */
    multiboot2_find_tags(mb2, MB_TAG_MEMORY, (tag_handler)callback, binf);
    multiboot2_find_tags(mb2, MB_TAG_MMAP, (tag_handler)callback, binf);
    return true;
}