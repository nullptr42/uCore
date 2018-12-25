/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_GDT_H_
#define _X86_64_PC_GDT_H_

/* TODO: Get rid of this file? */

#define SEG_KCODE_LM (0x08)
#define SEG_UCODE_LM (0x1B)
#define SEG_KDATA    (0x10)
#define SEG_UDATA    (0x23)

/**
 * @brief Initialize the Global Descriptor Table.
 */
void gdt_init();

#endif //_X86_64_PC_GDT_H_
