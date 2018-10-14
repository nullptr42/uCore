/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>

#include "gdt.h"
#include "console.h"
#include "bootinfo.h"
#include "apic/pic.h"
#include "apic/apic.h"

#include <log.h>
#include <lib/lib.h>
#include <version.h>
#include <arch/x86_64/cpuid.h>

void fpu_init();
void idt_init();
void pm_init();
void vm_init();

/* processor information retrieved via cpuid */
static struct amd64_cpu cpu;

/* information passed by the bootloader */
static struct bootinfo bootinfo;

void kernel_main(uint32_t magic, struct mb2_info* mb) {
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

    /* Ensure we have been booted via Multiboot2. */
    if (magic != MB2_BOOTLOADER_MAGIC) {
        error("bootinfo: bootloader isn't Multiboot2 compliant (magic number mismatch).");
        return;
    }

    /* Convert Multiboot2 information into our internal format. */
    if (!bootinfo_from_mb2(&bootinfo, mb)) {
        error("bootinfo: unable to create \"bootinfo\" structure from Multiboot2 data.");
        return;
    }

    /* Get important information about the processor. */
    cpuid_read(&cpu);

    pm_init();

    /* Setup interrupt controller and bootstrap other cores if possible... */
    if (lapic_is_present(&cpu)) {
        /* Disable PIC and initialize APIC. */
        pic_set_mask(0xFFFF);
        lapic_init();
    } else {
        /* Enable all interrupts */
        pic_set_mask(0);
    }

    /* Setup paging */
    vm_init();

    asm("sti");
    for(;;) asm("hlt");
}
