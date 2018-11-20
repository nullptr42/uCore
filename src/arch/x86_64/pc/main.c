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
#include "vm/vm.h"

#include <log.h>
#include <lib/lib.h>
#include <lib/vt100-codes.h>
#include <version.h>
#include <arch/x86_64/cpuid.h>

#include <libslab/slab.h>

void fpu_init();
void idt_init();

/* processor information retrieved via cpuid */
static struct amd64_cpu cpu;

/* information passed by the bootloader */
static struct bootinfo bootinfo;

struct myobj {
    uint64_t r[16];
    uint64_t pc;
};

void* objs[10000];
struct slab_cache myobj_cache;

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
        "Copyright (C) %d %s.\n"
        COLOR_CYAN "%s " COLOR_B_WHITE "Kernel %d.%d\n\n" CON_RESET,
        _k_copyright_year,
        _k_copyright_holder,
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

    libslab_init(&myobj_cache, sizeof(struct myobj));

    for (int i = 0; i < 10000; i++) {
        objs[i] = libslab_alloc(&myobj_cache);
        //trace("libslab: allocated object @ 0x%08x (%d)", objs[i], i);
    }
    for (int i = 0; i < 2; i++) {
        libslab_free(&myobj_cache, objs[i]);
        //trace("libslab: freed object @ 0x%08x (%d)", objs[i], i);
    }
    for (int i = 0; i < 1; i++) {
        objs[i] = libslab_alloc(&myobj_cache);
        trace("libslab: allocated object @ %p (%d)", objs[i], i);
    }

    trace("libslab: test completed :)");

    asm("sti");
    for(;;) asm("hlt");
}

void ap_main() {
    /* Initialize FPU and SSE. */
    fpu_init();

    kprint(COLOR_B_MAGENTA);
    kprint("My birth, your misery.\nMy amusement, your haunt...\n");
    kprint("I am the bringer of storm.\nI reign from deadlock hell.\n");
    kprint("They call me Core #2.\nFear me or core dump.\n");

    asm("sti");
    for (;;) asm("hlt");
}