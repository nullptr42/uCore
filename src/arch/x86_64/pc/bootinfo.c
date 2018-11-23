/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "bootinfo.h"
#include <arch/print.h>
#include <stddef.h>
#include <log.h>

struct state {
    struct bootinfo* binf;
    bool has_mmap;
    bool has_memory;
};

static bool callback_args(struct mb2_tag* tag, struct state* state) {
    struct bootinfo* binf = state->binf;

    size_t length = tag->size - sizeof(struct mb2_tag);
    char* cmdline = (void*)tag + sizeof(struct mb2_tag);
    char* dst = &binf->cmdline[0];
            
    if (length > MAX_CMDLINE_LEN+1) {
        klog(LL_WARN, "bootinfo: cmdline exceeds maximum length, truncating...\n");
        length = MAX_CMDLINE_LEN;
        dst[MAX_CMDLINE_LEN] = '\0';
    }
            
    while (length--) *dst++ = *cmdline++;

    return true;
}

static bool callback_memory(struct mb2_tag* tag, struct state* state) {
    struct bootinfo* binf = state->binf;

    binf->memory_lo = ((struct mb2_memory_tag*)tag)->mem_lower * 4096;
    binf->memory_hi = ((struct mb2_memory_tag*)tag)->mem_upper * 4096;
    state->has_memory = true;
    return true;
} 

static bool callback_mmap(struct mb2_tag* tag, struct state* state) {
    struct bootinfo* binf = state->binf;
    struct mb2_mmap_tag* mmap  = (void*)tag;

    struct memory* memory = &binf->mmap[0];
    uint32_t size = (uint32_t)sizeof(struct mb2_mmap_tag);
    struct mb2_mmap_ent* entry = (void*)mmap + size;

    state->has_mmap = true;

    /* Number of entries is defined through entry size and total structure size */
    while (size < tag->size) {
        if (entry->type == MB_MMAP_AVAILABLE) {
            /* Ensure the buffer will not be overrun */
            if (binf->num_mmap >= MAX_MMAP) {
                klog(LL_WARN, "bootinfo: number of Multiboot2 MMAP entries exceeds maximum (%d)\n", MAX_MMAP);
                return true;
            }

            /* Copy start and end address into bootinfo structure */
            memory->base = entry->base;
            memory->last = entry->base + entry->length - 1;

            memory++;
            binf->num_mmap++;
        }
                
        /* Update size and entry address */
        size += mmap->entry_size;
        entry = (void*)entry + mmap->entry_size;
    }
            
    return true;
}

static bool callback_module(struct mb2_tag* tag, struct state* state) {
    struct bootinfo* binf = state->binf;

    if (binf->num_modules == MAX_MODULES) {
        klog(LL_WARN, "bootinfo: number of modules exceeds maximum (%d)\n", MAX_MODULES);
        return true;
    }

    struct module* module = &binf->modules[binf->num_modules++];
    struct mb2_module_tag* entry = (void*)tag;
            
    module->base = (void*)(uint64_t)entry->mod_start;
    module->last = (void*)(uint64_t)entry->mod_end;

    int i = 0;
    char* string = &entry->string;
            
    while (string[i]) {
        if (i == MAX_MODULE_NAME_LEN) {
            klog(LL_WARN, "bootinfo: module string length exceeds %d characters.\n", MAX_MODULE_NAME_LEN);
            break;
        }
        module->name[i] = string[i];
        i++;
    }

    module->name[i] = '\0';
    return false;
}

static bool callback_fb(struct mb2_tag* tag, struct state* state) {
    struct bootinfo* binf = state->binf;
    struct mb2_fb_tag* fb = (void*)tag;

    /* We do not support palettes at the moment */
    if (fb->type == 0)
        return true;

    /* Decode basic info */
    binf->fb.present = true;
    binf->fb.address = (void*)fb->address;
    binf->fb.pitch = fb->pitch;
    binf->fb.width = fb->width;
    binf->fb.height = fb->height;
    binf->fb.bpp = fb->bpp;
    binf->fb.type = fb->type;

    /* Decode color encoding */
    binf->fb.format.r_shift = fb->r_shift;
    binf->fb.format.g_shift = fb->g_shift;
    binf->fb.format.b_shift = fb->b_shift;
    binf->fb.format.r_mask_len = fb->r_mask_len;
    binf->fb.format.g_mask_len = fb->g_mask_len;
    binf->fb.format.b_mask_len = fb->b_mask_len;
    
    return true;
}

bool bootinfo_from_mb2(struct bootinfo* binf, struct mb2_info* mb2) {
    struct state state = {
        .binf = binf,
        .has_mmap = false,
        .has_memory = false
    };
    
    /* Initialize basic data */
    binf->num_modules = 0;
    binf->num_mmap = 0;
    binf->cmdline[0] = '\0';
    binf->fb.present = false;

    /* Retrieve memory information */
    multiboot2_find_tags(mb2, MB_TAG_MEMORY, (tag_handler)callback_memory, &state);
    multiboot2_find_tags(mb2, MB_TAG_MMAP, (tag_handler)callback_mmap, &state);

    /* Retrieve command line */
    multiboot2_find_tags(mb2, MB_TAG_CMDLINE, (tag_handler)callback_args, &state);

    /* Retrieve module information */
    multiboot2_find_tags(mb2, MB_TAG_MODULE, (tag_handler)callback_module, &state);

    /* Retrieve any framebuffer information */
    multiboot2_find_tags(mb2, MB_TAG_FRAMEBUFFER, (tag_handler)callback_fb, &state);

    /* Check for missing information */
    if (!state.has_memory) {
        klog(LL_ERROR, "bootinfo: memory boundaries missing from Multiboot2 header.\n");
        return false;
    }
    if (!state.has_mmap) {
        klog(LL_ERROR, "bootinfo: memory map missing from Multiboot2 header.\n");
        return false;
    }

    return true;
}
