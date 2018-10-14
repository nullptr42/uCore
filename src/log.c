/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdarg.h>
#include <arch/print.h>
#include "log.h"

static int log_mask = LL_TRACE | LL_INFO | LL_WARN | LL_ERROR;

void set_log_mask(int mask) {
    log_mask = mask;
}

int get_log_mask() {
    return log_mask;
}

void trace(const char* format, ...) {
    if (~log_mask & LL_TRACE)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint("[trace] ");
    vkprintf(format, arg);
    kputc('\n');

    va_end(arg);
}

void info(const char* format, ...) {
    if (~log_mask & LL_INFO)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint("[info]  ");
    vkprintf(format, arg);
    kputc('\n');

    va_end(arg);
}

void warn(const char* format, ...) {
    if (~log_mask & LL_WARN)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint("[warn]  ");
    vkprintf(format, arg);
    kputc('\n');

    va_end(arg);
}

void error(const char* format, ...) {
    if (~log_mask & LL_ERROR)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint("[error] ");
    vkprintf(format, arg);
    kputc('\n');

    va_end(arg);
}
