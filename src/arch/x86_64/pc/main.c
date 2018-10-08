/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>

#include "gdt.h"
#include "console.h"
#include <lib/lib.h>
#include <version.h>

void idt_init();

void kernel_main() {
    gdt_init();
    idt_init();
    print_init();

    kprintf("%s %d.%d\n\n", _k_name, _k_version_major, _k_version_minor);
    
    asm("sti");
    for(;;) asm("hlt");
}
