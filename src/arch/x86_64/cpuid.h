/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_CPUID_H_
#define _X86_64_CPUID_H_

#include <stdint.h>

struct cpuid_result {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} __attribute__((packed));

enum cpuid_function {
    CPUID_GETVENDORSTRING,
    CPUID_GETFEATURES
};

void _cpuid(enum cpuid_function function, struct cpuid_result* result);

#endif //_X86_64_CPUID_H_
