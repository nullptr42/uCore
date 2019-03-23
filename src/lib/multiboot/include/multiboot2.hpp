/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _MULTIBOOT2_H_
#define _MULTIBOOT2_H_

#include <stdint.h>
#include <stdbool.h>

#define MB2_BOOTLOADER_MAGIC (0x36D76289)

struct mb2_info {
    uint32_t total_size;
    uint32_t reserved;
} __attribute__((packed));

enum mb2_tag_type {
    MB_TAG_CMDLINE = 1,
    MB_TAG_MODULE  = 3,
    MB_TAG_MEMORY  = 4,
    MB_TAG_MMAP    = 6,
    MB_TAG_FRAMEBUFFER = 8
};

struct mb2_tag {
    uint32_t type;
    uint32_t size;
} __attribute__((packed));

struct mb2_module_tag : mb2_tag {
    uint32_t mod_start;
    uint32_t mod_end;
    char string; /* array */
} __attribute__((packed));

struct mb2_memory_tag : mb2_tag {
    /* in kilobytes */
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((packed));

struct mb2_mmap_tag : mb2_tag {
    uint32_t entry_size;
    uint32_t entry_version;
} __attribute__((packed));

enum mb2_mmap_type {
    MB_MMAP_RESERVED  = 0,
    MB_MMAP_AVAILABLE = 1,
    MB_MMAP_ACPI_INFO = 3,
    MB_MMAP_HIBERNATE = 4,
    MB_MMAP_DEFECTIVE = 5
};

struct mb2_mmap_ent {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed));

struct mb2_fb_tag : mb2_tag {
    uint64_t address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    uint8_t  type;
    uint16_t reserved; /* according to documentation this should
                        * be uint8_t but it only works like this ¯\_(ツ)_/¯ */

    /* TFW assuming we are only using non-indexed mode. ;) */
    uint8_t r_shift;
    uint8_t r_mask_len;
    uint8_t g_shift;
    uint8_t g_mask_len;
    uint8_t b_shift;
    uint8_t b_mask_len;
} __attribute__((packed));

bool multiboot2_verify(uint32_t magic);

typedef bool (*tag_handler)(struct mb2_tag* tag, void* user_argument);

void multiboot2_find_tags(struct mb2_info* info,
                          enum mb2_tag_type type,
                          tag_handler handler,
                          void* user_argument
                         );

#endif // _MULTIBOOT2_H_
