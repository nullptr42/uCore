/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_MP_H_
#define _X86_64_PC_MP_H_

#include <stdint.h>

#define MP_MAX_CORES (256)

/* Multiprocessor Processor Entry */
struct mpc_cpu {
    uint8_t type;
    uint8_t lapic_id;
    uint8_t lapic_ver;
    unsigned int enabled : 1;
    unsigned int bsp : 1;
    unsigned int reserved : 6;
    uint32_t signature;
    uint32_t features;
} __attribute__((packed));

extern int mp_core_count;
extern struct mpc_cpu* mp_cores[MP_MAX_CORES];

void mp_init();

#endif //_X86_64_PC_MP_H_