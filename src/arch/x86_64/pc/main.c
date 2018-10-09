/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>

#include "gdt.h"
#include "console.h"
#include "multiboot/multiboot2.h"
#include "apic/pic.h"

#include <lib/lib.h>
#include <version.h>
#include <arch/x86_64/apic.h>
#include <arch/x86_64/cpuid.h>

void fpu_init();
void idt_init();
void rdmsr(uint32_t reg, uint32_t* eax, uint32_t* edx);
void pg_init();

void kernel_main(uint32_t magic, struct mb2_info* info) {
    (void)info;

    /* Enable FPU and SSE */
    fpu_init();

    /* Setup flat segmentation and interrupt vector table. */
    gdt_init();
    idt_init();
    pic_init();

    print_init();
    kprintf(
        "%s Microkernel %d.%d\nCopyright (C) 2018-present Frederic Raphael Meyer\n\n",
        _k_name,
        _k_version_major,
        _k_version_minor
    );

    struct amd64_cpu cpu;

    /* Get important information about the processor. */
    cpuid_read(&cpu);
    kprintf("cpu: vendor: %s\n", cpu.vendor);
    kprintf("cpu: features: %#llx\n\n", cpu.features);

    /* Ensure we have been booted via Multiboot2. */
    if (magic != MB2_BOOTLOADER_MAGIC) {
        kprint("ERROR: uCore must be booted via a Multiboot2-compliant bootloader.");
        return;
    }

    if (cpu.features & CPUID_FEAT_APIC) {
        uint32_t lapic_base_eax;
        uint32_t lapic_base_edx;
        uint64_t lapic_base;

        kprint("[TRACE] APIC support detected.\n");

        /* Read and print address of the local apic mmio base */
        rdmsr(0x1B, &lapic_base_eax, &lapic_base_edx);
        lapic_base = ((uint64_t)lapic_base_edx<<32)|lapic_base_eax;
        kprintf("[TRACE] Local-APIC base=0x%016llX\n", lapic_base);

        /* Disable all interrupts and initialize APIC. */
        pic_set_mask(0xFFFF);
        apic_init();
    } else {
        /* Enable all interrupts */
        pic_set_mask(0);
    }

    /* Setup paging */
    pg_init();

    asm("sti");
    for(;;) asm("hlt");
}
