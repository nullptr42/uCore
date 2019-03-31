/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <stdarg.h>
#include <stddef.h>

namespace cxx {

int vsnprintf(char *buffer, size_t size, const char *format, va_list arg);
int snprintf(char *buffer, size_t size, const char *format, ...);

void putc(char c);
void puts(const char *str);
int printf(const char *format, ...);
int vprintf(const char *format, va_list arg);

} // namespace cxx