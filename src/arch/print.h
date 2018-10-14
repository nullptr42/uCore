/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _ARCH_H_
#define _ARCH_H_

#include <stdarg.h>

void kputc(char c);
void kprint(const char* str);
int  kprintf(const char* format, ...);
int vkprintf(const char* format, va_list arg);

#endif //_ARCH_H_
