/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_CPUID_H_
#define _X86_64_CPUID_H_

#include <stdint.h>
#include <stdbool.h>

#include "features.h"

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

enum amd64_cpu_vendor {
    VENDOR_AMD,
    VENDOR_INTEL,
    VENDOR_UNSUPPORTED
};

struct amd64_cpu {
    uint32_t highest_func;
    char vendor[13];
    enum amd64_cpu_vendor vendor_id;

    struct processor {
        int stepping;
        int model;
        int family;
        int type;
        int ext_model;
        int ext_family;
    } processor;

    struct misc {
        int brand_id;
        int clflush_size;
        int logical_cpus;
        int apic_id;
    } misc;

    uint64_t features;
};

void _cpuid(enum cpuid_function function, struct cpuid_result* result);
void cpuid_read(struct amd64_cpu* cpu);

#endif //_X86_64_CPUID_H_
