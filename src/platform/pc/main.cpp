/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/x86_64/init.hpp>
#include <arch/x86_64/pm/pm.hpp>
#include <kernel/bootinfo.hpp>
#include <kernel/memory/virtual/virtual_alloc.hpp>
#include <lib/rxx/list.hpp>
#include <lib/rxx/stdio.hpp>
#include <platform/print.hpp>
#include <reinix/types.h>

#include "console.hpp"

using namespace arch::x86_64;

extern "C" void __cxa_pure_virtual() {
  while (1) {
  }
}

static Console console;
static terminal::Emulator emulator(console);

void platform::print(const char *string) { emulator.Write(string); }

//kernel::BootInfo *g_bootinfo = nullptr;
kernel::BootInfo *get_bootinfo(uint32_t magic, void *multiboot);

//PhysicalMemoryAllocator *g_pma;

extern "C" void kernel_main(uint32_t magic, void *multiboot) {
  auto bootinfo = get_bootinfo(magic, multiboot);
  
  arch::x86_64::initialize(bootinfo);
  
//  /* Print kernel name and version. */
//  rxx::printf("\e[2;37m%s\e[0m %d.%d\n\n", kernel::g_kernel_info.name,
//              kernel::g_kernel_info.version.major,
//              kernel::g_kernel_info.version.minor);
  

//  uint64_t pgs[11];
//  g_pma->Alloc(11, pgs);
//  for (int i = 0; i < 11; i++) {
//    rxx::printf("%d ", pgs[i]);
//  }
//
//  kernel::VirtualRangeAllocator alloc{0x0000000000001000, 0xFFFF7FFFFFFFFFFF,
//                                      4096};
//  rxx::printf("\n");
//  rxx::printf("alloc: 0x%016llx\n", (void *)alloc.Alloc(0xDEADBEEF, 1337));
//  rxx::printf("alloc: 0x%016llx\n", (void *)alloc.Alloc(0xFEEDFACE, 0x1000000));
//  rxx::printf("free: 0x%016llx\n", (void *)alloc.Free(0xFEEDFACE, 0x1000000));

  rxx::printf("halting...\n");
  
  asm("sti");
  for (;;)
    asm("hlt");
}
