/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_VM_H_
#define _X86_64_PC_VM_H_

#include <stdint.h>

#define PT_MAPPED         (1<<0)
#define PT_WRITEABLE      (1<<1)
#define PT_USERSPACE      (1<<2)
#define PT_WRITE_THROUGH  (1<<3)
#define PT_DISABLE_CACHE  (1<<4)
#define PT_WAS_USED       (1<<5)
#define PT_HUGE_PAGES     (1<<7)
#define PT_NO_EXECUTE     (1LL<<63)

#define __pgalign __attribute__((aligned(0x1000)))

#define KERNEL_VBASE (0xFFFFFFFF80000000)

typedef uint64_t ptentry_t;

/* Virtual memory context */
struct vm_context {
    ptentry_t pml4[512];
};

void vm_init();
void vm_alloc_init();

void vm_map_page(void* virtual, uint32_t page);
void vm_map_pages(void* virtual, uint32_t* pages, int num);
void vm_map_block(void* virtual, void* physical, uint64_t size);

void* vm_alloc(int count);
void  vm_free(void* virtual, int count);

extern ptentry_t* pml4_active;

#endif //_X86_64_PC_VM_H_