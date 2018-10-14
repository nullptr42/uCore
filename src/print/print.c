/*
 * Copyright (C) 2017-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "print.h"

#include <lib/lib.h>

static int num_drivers = -1;
static struct print_driver** drivers = NULL;

static inline void _print(char* str) {
    if (num_drivers < 1 || drivers == NULL)
        return;

    for (int i = 0; i < num_drivers; i++)
        drivers[i]->print(str);
}

static inline void _print_char(char c) {
    if (num_drivers < 1 || drivers == NULL)
        return;

    for (int i = 0; i < num_drivers; i++)
        drivers[i]->print_char(c);
}

void print_set_drivers(struct print_driver** _drivers, int _num_drivers) {
    drivers     = _drivers;
    num_drivers = _num_drivers;
}

void print_char(char c) {
    _print_char(c);
}

void print(char* str) {
    _print(str);
    _print_char('\n');
}

int print_fmt_va(const char* format, va_list args) {
    // first call is to calculate the required buffer size.
    int count = vsnprintf(NULL, 0, format, args);

    // if count is negative, something went wrong and we don't need to proceed.
    if (count >= 0) {
        char buffer[count+1];

        vsnprintf(buffer, count+1, format, args);
        _print(buffer);

        va_end(args);
        return 0;
    }
    return -1;
}

int print_fmt(const char* format, ...) {
    va_list args;
    va_start(args, format);

    int result = print_fmt_va(format, args);

    va_end(args);

    return result;
}