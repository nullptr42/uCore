/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/x86_64/cpuid.hpp>
#include <arch/x86_64/gdt.hpp>
#include <arch/x86_64/idt.hpp>
#include <arch/x86_64/pic.hpp>
#include <kernel/bootinfo.hpp>
#include <kernel/version.hpp>
#include <lib/cxx/list.hpp>
#include <lib/cxx/stdio.hpp>
#include <platform/print.hpp>
#include <stdint.h>

#include "console.hpp"

extern "C" void fpu_init();

using namespace arch::x86_64;

extern "C" void __cxa_pure_virtual() {
  while (1) {
  }
}

static Console console;
static terminal::Emulator emulator(console);

void platform::print(const char *string) { emulator.Write(string); }

kernel::BootInfo *g_bootinfo = nullptr;
kernel::BootInfo *get_bootinfo(uint32_t magic, void *multiboot);

extern "C" void kernel_main(uint32_t magic, void *multiboot) {
  cpuid::CPU cpu;

  fpu_init();

  /* Setup flat segmentation and interrupt vector table. */
  gdt::initialize();
  idt::initialize();
  pic::initialize();

  /* Print kernel name and version. */
  cxx::printf("\e[2;37m%s\e[0m %d.%d\n\n", kernel::g_kernel_info.name,
              kernel::g_kernel_info.version.major,
              kernel::g_kernel_info.version.minor);

  cpuid::read(cpu);
  cxx::printf("%s (%s)\n\n", cpu.name, cpu.vendor_name);

  g_bootinfo = get_bootinfo(magic, multiboot);

  if (cpu.features & uint64_t(cpuid::Feature::APIC)) {
    pic::set_mask(0xFFFF);
    /* TODO: Initialize the APIC. */
  } else {
    pic::set_mask(0);
  }

  asm("sti");
  for (;;)
    asm("hlt");
}
