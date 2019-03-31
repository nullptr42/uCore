/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <stdarg.h>
#include <stdio.hpp>

/* TODO: Implement a proper panic function. */
inline void panic(const char *format, ...) {
  va_list arg1;
  va_start(arg1, format);
  cxx::printf("\e[2;41;37mpanic\e[0m: ");
  cxx::vprintf(format, arg1);
  va_end(arg1);
  while (1) {
  }
}