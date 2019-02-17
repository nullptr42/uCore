/*
 * Copyright (C) 2019 Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _K_LIB_LIBC_STDIO_H_
#define _K_LIB_LIBC_STDIO_H_

#include <stddef.h>
#include <stdarg.h>

int vsnprintf(char* buffer, size_t size, const char* format, va_list arg);
int snprintf(char* buffer, size_t size, const char* format, ...);

void putc(char c);
void puts(const char* str);
int  printf(const char* format, ...);
int vprintf(const char* format, va_list arg);


#endif //_K_LIB_LIBC_STDIO_H_