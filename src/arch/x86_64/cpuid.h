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

/* Derived from: https://wiki.osdev.org/CPUID 
 * CHECKME: Is this a complete list? 
 */
#define CPUID_FEAT_SSE3         (1<<0) 
#define CPUID_FEAT_PCLMUL       (1<<1)
#define CPUID_FEAT_DTES64       (1<<2)
#define CPUID_FEAT_MONITOR      (1<<3)  
#define CPUID_FEAT_DS_CPL       (1<<4)  
#define CPUID_FEAT_VMX          (1<<5)  
#define CPUID_FEAT_SMX          (1<<6)  
#define CPUID_FEAT_EST          (1<<7)  
#define CPUID_FEAT_TM2          (1<<8)  
#define CPUID_FEAT_SSSE3        (1<<9)  
#define CPUID_FEAT_CID          (1<<10)
#define CPUID_FEAT_FMA          (1<<12)
#define CPUID_FEAT_CX16         (1<<13) 
#define CPUID_FEAT_ETPRD        (1<<14) 
#define CPUID_FEAT_PDCM         (1<<15) 
#define CPUID_FEAT_PCIDE        (1<<17) 
#define CPUID_FEAT_DCA          (1<<18) 
#define CPUID_FEAT_SSE4_1       (1<<19) 
#define CPUID_FEAT_SSE4_2       (1<<20) 
#define CPUID_FEAT_x2APIC       (1<<21) 
#define CPUID_FEAT_MOVBE        (1<<22) 
#define CPUID_FEAT_POPCNT       (1<<23) 
#define CPUID_FEAT_AES          (1<<25) 
#define CPUID_FEAT_XSAVE        (1<<26) 
#define CPUID_FEAT_OSXSAVE      (1<<27) 
#define CPUID_FEAT_AVX          (1<<28)
#define CPUID_FEAT_FPU          (1<<32)  
#define CPUID_FEAT_VME          (1LL<<33)  
#define CPUID_FEAT_DE           (1LL<<34)  
#define CPUID_FEAT_PSE          (1LL<<35)  
#define CPUID_FEAT_TSC          (1LL<<36)  
#define CPUID_FEAT_MSR          (1LL<<37)  
#define CPUID_FEAT_PAE          (1LL<<38)  
#define CPUID_FEAT_MCE          (1LL<<39)  
#define CPUID_FEAT_CX8          (1LL<<40)  
#define CPUID_FEAT_APIC         (1LL<<41)  
#define CPUID_FEAT_SEP          (1LL<<43) 
#define CPUID_FEAT_MTRR         (1LL<<44) 
#define CPUID_FEAT_PGE          (1LL<<45) 
#define CPUID_FEAT_MCA          (1LL<<46) 
#define CPUID_FEAT_CMOV         (1LL<<47) 
#define CPUID_FEAT_PAT          (1LL<<48) 
#define CPUID_FEAT_PSE36        (1LL<<49) 
#define CPUID_FEAT_PSN          (1LL<<50) 
#define CPUID_FEAT_CLF          (1LL<<51) 
#define CPUID_FEAT_DTES         (1LL<<53) 
#define CPUID_FEAT_ACPI         (1LL<<54) 
#define CPUID_FEAT_MMX          (1LL<<55) 
#define CPUID_FEAT_FXSR         (1LL<<56) 
#define CPUID_FEAT_SSE          (1LL<<57) 
#define CPUID_FEAT_SSE2         (1LL<<58) 
#define CPUID_FEAT_SS           (1LL<<59) 
#define CPUID_FEAT_HTT          (1LL<<60) 
#define CPUID_FEAT_TM1          (1LL<<61) 
#define CPUID_FEAT_IA64         (1LL<<62)
#define CPUID_FEAT_PBE          (1LL<<63)


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
