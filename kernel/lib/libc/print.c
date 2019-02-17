/*
 * Copyright (C) 2019 Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>
#include <stdarg.h>
#include <platform/print.h>
#include <lib/libc/stdio.h>

void putc(char c) {
    char str[] =  { c, 0 };
    platform_print(str);
}

void puts(const char* str) {
    platform_print(str);
}

int printf(const char* format, ...) {
    va_list arg1, arg2;
    
    va_start(arg1, format);
    va_copy(arg2, arg1);
    
    int total = vsnprintf(NULL, 0, format, arg2);

    if (total > -1) {
        char buffer[total+1];

        vsnprintf(buffer, total+1, format, arg1);
        platform_print(buffer);
    }    

    va_end(arg2);    
    va_end(arg1);
    return total;
}

int vprintf(const char* format, va_list arg) {
    va_list arg2;
    va_copy(arg2, arg);
    
    int total = vsnprintf(NULL, 0, format, arg);

    if (total > -1) {
        char buffer[total+1];

        vsnprintf(buffer, total+1, format, arg2);
        platform_print(buffer);
    }

    va_end(arg2);
    return total;
}
