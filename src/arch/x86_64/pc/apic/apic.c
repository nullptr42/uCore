
/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "apic.h"
#include <stdint.h>
#include <stddef.h>
#include <arch/print.h>
#include <arch/x86_64/pc/vm/vm.h>

#define MSR_APIC_BASE  (0x1B)
#define MSR_APIC_BASE_ENABLE (0x800)
#define MSR_APIC_BASE_BSP (0x100)

void _rdmsr(uint32_t reg, uint32_t* eax, uint32_t* edx);
void _wrmsr(uint32_t reg, uint32_t  eax, uint32_t  edx);

static void* lapic_mmio = NULL;

bool lapic_is_present(struct amd64_cpu* cpu) {
    return cpu->features & CPUID_FEAT_APIC;
}

static uint64_t lapic_get_base() {
    uint32_t lapic_base_edx;
    uint32_t lapic_base_eax;

    /* get current LAPIC base from MSR */
    _rdmsr(MSR_APIC_BASE, &lapic_base_eax, &lapic_base_edx);

    return ((uint64_t)lapic_base_edx<<32)|(lapic_base_eax&0xFFFFF000);
}

static void lapic_set_base(uint64_t base) {
    base = (base&0xFFFFF000)|MSR_APIC_BASE_ENABLE|MSR_APIC_BASE_BSP;
    _wrmsr(MSR_APIC_BASE, (base&0xFFFFFFFF), (base>>32));
}

static void find_mpc_table() {
    void* data = (void*)0xFFFF808000000000;

    /* TODO: atm we are scanning the entire first 1MiB.
     * However the specification allows us to narrow it down
     * to three specific memory areas.
     */
    for (int i = 0; i <= 0xFFFFC; i++) {
        if (*(uint32_t*)(data + i) == 0x5F504D5F) { /* _MP_ */
            info("lapic: Floating Pointer Structure @ %p", data + i);
            return;
        }        
    }
}

void lapic_init() {
    uint64_t base = lapic_get_base();

    /* Enable the Local-APIC */
    info("lapic: base address is 0x%016llX", base);
    lapic_set_base(base);
    info("lapic: enabled Local-APIC.");

    /* Map its MMIO registers into memory */
    lapic_mmio = vm_alloc(1);
    vm_map_page(lapic_mmio, base / 4096);
    info("lapic: mapped Local-APIC MMIO @ %p", lapic_mmio);

    volatile uint32_t* spivr = (void*)lapic_mmio + 0xF0;

    /* Actually enabling the Local-APIC by setting
     * Enable-bit in the SPIVR register.
     */    
    *spivr |= 0x80;

    find_mpc_table();
}
