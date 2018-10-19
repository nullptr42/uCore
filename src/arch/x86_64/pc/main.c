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
#include "pm/pm.h"

#include <log.h>
#include <lib/lib.h>
#include <version.h>
#include <arch/x86_64/cpuid.h>

void fpu_init();
void idt_init();
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

#if 0
    /* Framebuffer debug */
    info("framebuffer: address=%p", bootinfo.fb.address);
    info("framebuffer: pitch=%lu", bootinfo.fb.pitch);
    info("framebuffer: width=%lu", bootinfo.fb.width);
    info("framebuffer: height=%lu", bootinfo.fb.height);
    info("framebuffer: bpp=%u", bootinfo.fb.bpp);
    info("framebuffer: type=%u", bootinfo.fb.type);
    info("framebuffer: r: shift=%u len=%u", bootinfo.fb.format.r_shift, bootinfo.fb.format.r_mask_len);
    info("framebuffer: g: shift=%u len=%u", bootinfo.fb.format.g_shift, bootinfo.fb.format.g_mask_len);
    info("framebuffer: b: shift=%u len=%u", bootinfo.fb.format.b_shift, bootinfo.fb.format.b_mask_len);
#endif

    /* Get important information about the processor. */
    cpuid_read(&cpu);

    /* Initialize the Physical Memory Manager */
    pm_init(&bootinfo);

    /* Test physical memory manager */
    uint32_t pages[6];
    if (pm_stack_alloc(6, pages) != PMM_OK)
        error("main: something went wrong...");
    for (int i = 0; i < 6; i++)
        kprintf("%d\n", pages[i]);

    /* Initialize the Virtual Memory Manager */
    vm_init();

    /* Setup interrupt controller and bootstrap other cores if possible... */
    if (lapic_is_present(&cpu)) {
        /* Disable PIC and initialize APIC. */
        pic_set_mask(0xFFFF);
        lapic_init();
    } else {
        /* Enable all interrupts */
        pic_set_mask(0);
    }

    asm("sti");
    for(;;) asm("hlt");
}
