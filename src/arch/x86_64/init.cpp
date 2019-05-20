/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "init.hpp"

#include <arch/x86_64/cpuid.hpp>
#include <arch/x86_64/gdt.hpp>
#include <arch/x86_64/idt.hpp>
#include <arch/x86_64/pic.hpp>

extern "C" void fpu_init();

namespace arch::x86_64 {

static void init_cpu() {
  fpu_init();
  
  cpuid::CPU cpu;
  
  /* Setup flat segmentation and interrupt vector table. */
  gdt::initialize();
  idt::initialize();
  pic::initialize();
  
  cpuid::read(cpu);
  
  /* TODO: is there really any x86_64 cpu without an APIC? */
  if (cpu.features & uint64_t(cpuid::Feature::APIC)) {
    pic::set_mask(0xFFFF);
    /* ... */
  } else {
    pic::set_mask(0);
  }
}
  
static void init_mm(kernel::BootInfo *bootinfo) {
  /* ... */
}

void initialize(kernel::BootInfo *bootinfo) {
  init_cpu();
  init_mm(bootinfo);
}

}