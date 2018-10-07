/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_CONSOLE_H_
#define _X86_64_CONSOLE_H_

void print_init();

void kputc(char c);
void kprint(const char* str);
int kprintf(const char* format, ...);

#endif //_X86_64_CONSOLE_H_