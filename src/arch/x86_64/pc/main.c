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

#include <lib/lib.h>
#include <version.h>
#include <arch/x86_64/cpuid.h>

void fpu_init();
void idt_init();
void pg_init();

/* processor information retrieved via cpuid */
static struct amd64_cpu cpu;

/* information passed by the bootloader */
static struct bootinfo bootinfo;

void kernel_main(uint32_t magic, struct mb2_info* info) {
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
        kprint("ERROR: uCore must be booted via a Multiboot2-compliant bootloader.");
        return;
    }

    /* Convert Multiboot2 information into our internal format. */
    if (!bootinfo_from_mb2(&bootinfo, info)) {
        kprint("ERROR: unable to create \"bootinfo\" structure.");
        return;
    }
    /*for (int i = 0; i < bootinfo.num_mmap; i++) {
        kprintf("[mmap] lo=%llp hi=%llp\n", bootinfo.mmap[i].base, bootinfo.mmap[i].last);
    }
    kprintf("[cmdline] %s\n", bootinfo.cmdline);
    for (int i = 0; i < bootinfo.num_modules; i++) {
        kprintf("[module] lo=%llp hi=%llp name=%s\n", bootinfo.modules[i].base, bootinfo.modules[i].last, bootinfo.modules[i].name);
    }*/

    /* Get important information about the processor. */
    cpuid_read(&cpu);

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
    pg_init();

    asm("sti");
    for(;;) asm("hlt");
}
