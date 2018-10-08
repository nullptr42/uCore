/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_APIC_H_
#define _X86_64_APIC_H_

#include <stdbool.h>

bool apic_is_present();
void apic_init();

#endif //_X86_64_APIC_H_