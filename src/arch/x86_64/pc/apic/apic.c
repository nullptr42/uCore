/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "mp.h"
#include "apic.h"

#include <stdint.h>
#include <stddef.h>
#include <arch/print.h>
#include <arch/x86_64/pc/pm/pm.h>
#include <arch/x86_64/pc/vm/vm.h>
#include <log.h>

#define MSR_APIC_BASE  (0x1B)
#define MSR_APIC_BASE_ENABLE (0x800)
#define MSR_APIC_BASE_BSP (0x100)

#define CPU_STACK_PAGES (8) /* 32 KiB */
#define CPU_STACK_SIZE  (CPU_STACK_PAGES * 4096)

bool core_wakedup;

static void* lapic_mmio = NULL;

void _rdmsr(uint32_t reg, uint32_t* eax, uint32_t* edx);
void _wrmsr(uint32_t reg, uint32_t  eax, uint32_t  edx);

bool lapic_is_present(struct amd64_cpu* cpu) {
    return cpu->features & CPUID_FEAT_APIC;
}

static uint64_t lapic_get_base() {
    uint32_t lapic_base_edx;
    uint32_t lapic_base_eax;

    /* Get current LAPIC base from MSR */
    _rdmsr(MSR_APIC_BASE, &lapic_base_eax, &lapic_base_edx);

    return ((uint64_t)lapic_base_edx<<32)|(lapic_base_eax&0xFFFFF000);
}

static void lapic_set_base(uint64_t base) {
    base = (base&0xFFFFF000)|MSR_APIC_BASE_ENABLE|MSR_APIC_BASE_BSP;
    _wrmsr(MSR_APIC_BASE, (base&0xFFFFFFFF), (base>>32));
}

static void wakeup(int apic_id) {
    volatile uint32_t* icr_hi = (void*)lapic_mmio + 0x310;
    volatile uint32_t* icr_lo = (void*)lapic_mmio + 0x300;
    volatile uint32_t* id = (void*)lapic_mmio + 0x20;

    core_wakedup = false;

    *icr_hi = apic_id << 24;
    *icr_lo = 0x08 | (5 << 8) | (1 << 14) | (0 << 18);
    *id;
    //delay(10);
    while (icr_lo[0] & (1<<12)) ;
    *icr_lo = 0x08 | (6 << 8) | (0 << 14) | (0 << 18);
    *id;
    //delay(10);
    while (icr_lo[0] & (1<<12)) ;

    /**
     * Wait for the CPU core to complete its initialization routine.
     * When the CPU core woke up it will set core_wakedup to true.
     * We might want to implement a giveup timeout. 
     */
    while (!core_wakedup) asm("nop;");
}

extern const void _wakeup_start;
extern const void _wakeup_end;
extern const void _wakeup_tab;

void panic();

void lapic_init() {
    uint64_t base = lapic_get_base();

    /* Enable the Local-APIC */
    lapic_set_base(base);

    /* Map its MMIO registers into memory. */
    lapic_mmio = vm_alloc(1);
    vm_map_page(lapic_mmio, base / 4096);
    klog(LL_DEBUG, "apic: Mapped Local-APIC MMIO @ %p", lapic_mmio);

    volatile uint32_t* spivr = (void*)lapic_mmio + 0xF0;

    /* Enable Local-APIC MMIO. */
    *spivr |= 0x80;

    uint64_t* src = (void*)&_wakeup_start;
    uint64_t* dst = (void*)VM_BASE_PHYSICAL + 0x8000;
    uint64_t* tab = (void*)dst + (&_wakeup_tab - &_wakeup_start);

    /* Copy payload to lower 1 MiB. */
    while (src < (uint64_t*)&_wakeup_end)
        *dst++ = *src++;

    /* Detect any present CPU cores. */
    mp_init();

    void* stack;

     /* Wakeup all detected CPU cores. */
    for (int i = 0; i < mp_core_count; i++) {
        /* Do not try to wakeup ourselves. */
        if (mp_cores[i]->bsp)
            continue;

        uint32_t pages[CPU_STACK_PAGES];
        
        if (pm_stack_alloc(CPU_STACK_PAGES, pages) != PMM_OK) {
            klog(LL_ERROR, "apic: cpu[%d]: Unable to allocate physical memory for CPU stack.", i);
            panic();
        }
        
        stack = vm_alloc(CPU_STACK_PAGES);

        if (stack == NULL) {
            klog(LL_ERROR, "apic: cpu[%d]: Unable to allocted virtul memory for CPU stack.", i);
            panic();
        }

        klog(LL_DEBUG, "apic: cpu[%d]: stack @ virtual %p.", i, stack);

        vm_map_pages(stack, pages, CPU_STACK_PAGES);
        tab[0] = (uint64_t)stack + CPU_STACK_SIZE;
        
        wakeup(mp_cores[i]->lapic_id);
    }
}
