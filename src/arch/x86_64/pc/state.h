/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_STATE_H_
#define _X86_64_PC_STATE_H_

#include <stdint.h>

struct cpu_state {
    // Segment Registers
    uint64_t ds;
    uint64_t fs;
    uint64_t gs;

    // General Purpose Registers
    uint64_t rax;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    // Interrupt Number and Error Code
    uint64_t intr;
    uint64_t error;

    // Registers saved by CPU when calling into the ISR.
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

#endif //_X86_64_PC_STATE_H_
