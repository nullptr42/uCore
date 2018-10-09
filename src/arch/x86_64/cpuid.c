/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "cpuid.h"

static enum amd64_cpu_vendor get_vendor_by_name(struct cpuid_result* regs) {
    /* Check for "GenuineIntel" */
    if (regs->ebx == 0x756e6547 && 
        regs->edx == 0x49656e69 &&
        regs->ecx == 0x6c65746e) return VENDOR_INTEL;

    /* Check for "AuthenticAMD" */
    if (regs->ebx == 0x68747541 &&
        regs->edx == 0x69746e65 &&
        regs->ecx == 0x444d4163) return VENDOR_AMD;
    
    return VENDOR_UNSUPPORTED;
}

void cpuid_read(struct amd64_cpu* cpu) {
    struct cpuid_result regs;

    /* Get vendor name and highest supported function */
    uint32_t* vendor = (void*)&cpu->vendor;
    _cpuid(CPUID_GETVENDORSTRING, &regs);
    vendor[0] = regs.ebx;
    vendor[1] = regs.edx;
    vendor[2] = regs.ecx;
    cpu->vendor[12] = '\0';
    cpu->vendor_id = get_vendor_by_name(&regs);
    cpu->highest_func = regs.eax;

    /* Get feature information */
    if (cpu->highest_func < CPUID_GETFEATURES)
        goto done;
    _cpuid(CPUID_GETFEATURES, &regs);
    cpu->features = ((uint64_t)regs.edx<<32)|regs.ecx;
    
    /* Decode processor information from eax */
    cpu->processor.stepping   = (regs.eax >>  0) & 0xF;
    cpu->processor.model      = (regs.eax >>  4) & 0xF;
    cpu->processor.family     = (regs.eax >>  8) & 0xF;
    cpu->processor.type       = (regs.eax >> 12) & 0x3;
    cpu->processor.ext_model  = (regs.eax >> 16) & 0xF;
    cpu->processor.ext_family = (regs.eax >> 20) & 0xFF;

    /* Decode miscellaneous information from ebx */
    cpu->misc.brand_id     = (regs.ebx >>  0) & 0xFF;
    cpu->misc.clflush_size = (regs.ebx >>  8) & 0xFF;
    cpu->misc.logical_cpus = (regs.ebx >> 16) & 0xFF;
    cpu->misc.apic_id      = (regs.ebx >> 24) & 0xFF;
    
done:
    /* TODO */
    return;
}