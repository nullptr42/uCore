/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_PM_STACK_H_
#define _X86_64_PC_PM_STACK_H_

#include "status.h"
#include <platform/pc/bootinfo.h>

struct pm_stack {
    uint32_t* pages;
    int pos;
    int len;
};

struct pm_stack* pm_get_stack();
bool pm_init_stack(struct bootinfo* binf, void* minimum);

enum pmm_status pm_stack_alloc(int num, uint32_t* pages);
enum pmm_status pm_stack_free(int num, uint32_t* pages);

#endif //_X86_64_PC_PM_STACK_H_
