/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_APIC_H_
#define _X86_64_PC_APIC_H_

#include <stdbool.h>
#include <arch/x86_64/cpuid/cpuid.h>

extern volatile bool core_wokeup;

bool lapic_is_present(struct amd64_cpu* cpu);
void lapic_init();

#endif //_X86_64_PC_APIC_H_
