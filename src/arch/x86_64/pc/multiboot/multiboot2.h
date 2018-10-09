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

struct mb2_tag {
    uint32_t type;
    uint32_t size;
} __attribute__((packed));

enum mb2_tag_type {
    MB_TAG_MODULE = 3
};

struct mb2_module_tag {
    struct mb2_tag tag;

    uint32_t mod_start;
    uint32_t mod_end;
    char string; /* array */
} __attribute__((packed));

bool multiboot2_verify(uint32_t magic);

typedef bool (*tag_handler)(struct mb2_tag* tag, void* user_argument);

void multiboot2_find_tags(struct mb2_info* info,
                          enum mb2_tag_type type,
                          tag_handler handler,
                          void* user_argument
                         );

#endif // _MULTIBOOT2_H_
