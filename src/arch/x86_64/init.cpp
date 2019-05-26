/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "init.hpp"
#include "global.hpp"

#include <arch/x86_64/cpu/apic/pic.hpp>
#include <arch/x86_64/cpu/cpuid.hpp>
#include <arch/x86_64/cpu/gdt.hpp>
#include <arch/x86_64/cpu/idt.hpp>
#include <arch/x86_64/mm/address_space.hpp>
#include <arch/x86_64/mm/frame_alloc.hpp>
#include <kernel/memory/memory.hpp>
#include <lib/rxx/stdio.hpp>

extern "C" void fpu_init();

/* Original PML4 table. */
extern "C" uint64_t boot_pml4;

extern "C" const uint8_t kernel_start;
extern "C" const uint8_t kernel_end;

namespace arch::x86_64 {

static ptentry_t new_pml4[512] __pgalign;

static void init_cpu() {
  fpu_init();

  cpuid::CPU cpu;

  /* Setup flat segmentation and interrupt vector table. */
  gdt::initialize();
  idt::initialize();
  pic::initialize();

  cpuid::read(cpu);
  rxx::printf("%s (%s)\n", cpu.name, cpu.vendor_name);

  /* TODO: is there really any x86_64 cpu without an APIC? */
  if (cpu.features & uint64_t(cpuid::Feature::APIC)) {
    pic::set_mask(0xFFFF);
    /* ... */
  } else {
    pic::set_mask(0);
  }
}

static void init_mm(kernel::BootInfo *bootinfo) {
  X64_AddressSpace aspace1{(ptentry_t *)physical(vaddr_t(&boot_pml4))};
  X64_AddressSpace aspace2{(ptentry_t *)physical(vaddr_t(&new_pml4[0]))};

  for (int i = 0; i < 512; i++)
    new_pml4[i] = 0;

  /* Setup recursive mapping to new PML4 in old and new PML4.
   * This way we can use standard mapping functions to map the kernel
   * to the new PML4 nicely and then switch to the new context.
   */
  (&boot_pml4)[256] = new_pml4[256] = X64_AddressSpace::PT_MAPPED |
                                      X64_AddressSpace::PT_WRITEABLE |
                                      physical(vaddr_t(new_pml4));

  auto frame_alloc = new X64_FrameAllocator();

  kernel::g_frame_alloc = frame_alloc;
  frame_alloc->Init(bootinfo);

  aspace1.Map(0xFFFFFFFF800B8000, 0xB8000, 0);

  aspace1.Map(vaddr_t(&kernel_start), paddr_t(physical(vaddr_t(&kernel_start))),
              size_t(&kernel_end - &kernel_start + 1), 0);

  frame_alloc->Map(aspace1, 0xFFFFFFFF00000000);
  aspace2.Bind();

  rxx::printf("Survived init_mm!\n");
}

void initialize(kernel::BootInfo *bootinfo) {
  init_cpu();
  init_mm(bootinfo);
}

} // namespace arch::x86_64