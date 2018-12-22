/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_BOOTINFO_H_
#define _X86_64_PC_BOOTINFO_H_

#include <stdint.h>
#include <stdbool.h>
#include "multiboot/multiboot2.h"

#define MAX_MODULES (256)
#define MAX_MMAP (256)
#define MAX_CMDLINE_LEN (2048) /* this most likely already is overkill */

#define MAX_MODULE_NAME_LEN (128)

/* Represents a loadable service executable */
struct module {
    char  name[MAX_MODULE_NAME_LEN+1];
    void* base;
    void* last;
} __attribute__((packed));

/* Represents a memory map entry */
struct memory {
    uint64_t base;
    uint64_t last;
} __attribute__((packed));

/* Store information collected by the Bootloader */
struct bootinfo {
    char cmdline[MAX_CMDLINE_LEN+1];

    int num_modules;
    int num_mmap;
    
    struct module modules[MAX_MODULES];

    uint64_t memory_lo;
    uint64_t memory_hi;
    struct memory mmap[MAX_MMAP];

    /* Generic framebuffer information */
    struct framebuffer {
        bool present;
        
        void* address;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t  bpp;
        uint8_t  type; /* hackish */

        /* Color format descriptor */
        struct format {
            uint8_t r_shift;
            uint8_t g_shift;
            uint8_t b_shift;
            uint8_t r_mask_len;
            uint8_t g_mask_len;
            uint8_t b_mask_len;
        } format;
    } fb;
};

bool bootinfo_from_mb2(struct bootinfo* binf, struct mb2_info* mb2);

#endif //_X86_64_PC_BOOTINFO_H_
