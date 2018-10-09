/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_H_
#define _X86_64_PC_H_

#include <stdint.h>

#define IRQ_BASE_M (0x20)
#define IRQ_BASE_S (0x28)

void pic_set_mask(uint16_t irq_mask);
void pic_init();

#endif //_X86_64_PC_H_
