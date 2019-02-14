/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>

#include "console.h"
#include "vm/vm.h"
#include "multiboot/multiboot2.h"

#include <bootinfo.h>
#include <log.h>
#include <lib/lib.h>
#include <lib/vt100-codes.h>
#include <version.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/cpuid/cpuid.h>
#include <arch/x86_64/apic/pic.h>
#include <arch/x86_64/apic/apic.h>
#include <arch/x86_64/pm/pm.h>

/* TODO: Place these in headers. */
void fpu_init();
void idt_init();
bool bootinfo_from_mb2(struct bootinfo* binf, struct mb2_info* mb2);

/* processor information retrieved via cpuid */
static struct amd64_cpu cpu;

/* information passed by the bootloader */
static struct bootinfo bootinfo;

static void fb_map() {
    void*    phys = bootinfo.fb.address;
    uint64_t size = bootinfo.fb.height * bootinfo.fb.pitch;

    int pg_count = size / 4096;

    if ((size % 4096) != 0)
        pg_count++;

    /* Allocate block of virtual memory and map framebuffer */
    bootinfo.fb.address = vm_alloc(pg_count);
    vm_map_block(bootinfo.fb.address, phys, size);
}

void kernel_main(uint32_t magic, struct mb2_info* mb) {
    /* Enable FPU and SSE */
    fpu_init();

    /* Setup flat segmentation and interrupt vector table. */
    gdt_init();
    idt_init();
    pic_init();

    print_init();
    kprintf(
        COLOR_CYAN "%s " CON_RESET "Kernel %d.%d\n\n" CON_RESET,
        _k_name,
        _k_version_major,
        _k_version_minor
    );

    /* Ensure we have been booted via Multiboot2. */
    if (magic != MB2_BOOTLOADER_MAGIC) {
        klog(LL_ERROR, "Bootloader isn't Multiboot2-compliant.");
        return;
    }

    /* Convert Multiboot2 information into our internal format. */
    if (!bootinfo_from_mb2(&bootinfo, mb)) {
        klog(LL_ERROR, "Unable to create \"bootinfo\" structure from Multiboot2 data.");
        return;
    }

    /* Get important information about the processor. */
    cpuid_read(&cpu);

    /* Initialize the Physical Memory Manager */
    pm_init(&bootinfo);

    /* Initialize the Virtual Memory Manager */
    vm_init();
    fb_map();

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

void ap_main() {
    /* Initialize FPU and SSE. */
    fpu_init();

    struct amd64_cpu _cpu;

    cpuid_read(&_cpu);
    kprintf(COLOR_B_YELLOW "cpu[%d]: woke up!\n" CON_RESET, _cpu.misc.apic_id);
    core_wokeup = true;

    asm("sti");
    for (;;) asm("hlt");
}
