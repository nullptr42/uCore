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

#define MAX_MODULES (255)
#define MAX_MMAP (255)
#define MAX_CMDLINE_LEN (2048) /* this probably already is overkill */

/* Represents a loadable service executable */
struct module {
    char* name;
    void* base;
    void* last;
} __attribute__((packed));

/* Represents a memory map entry */
struct memory {
    uint64_t base;
    uint64_t last;
} __attribute__((packed));

struct bootinfo {
    char cmdline[MAX_CMDLINE_LEN+1];

    int num_modules;
    int num_mmap;
    
    struct module modules[MAX_MODULES];

    uint64_t memory_lo;
    uint64_t memory_hi;
    struct memory mmap[MAX_MMAP];
};

bool bootinfo_from_mb2(struct bootinfo* binf, struct mb2_info* mb2);

#endif //_X86_64_PC_BOOTINFO_H_
