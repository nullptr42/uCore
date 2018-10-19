/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdarg.h>
#include <arch/print.h>
#include "log.h"
#include <lib/vt100-codes.h>

static int last_level = 0;
static int log_mask = LL_TRACE | LL_INFO | LL_WARN | LL_ERROR;

void set_log_mask(int mask) {
    log_mask = mask;
}

int get_log_mask() {
    return log_mask;
}

void trace(const char* format, ...) {
    last_level = LL_TRACE;

    if (~log_mask & LL_TRACE)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint(COLOR_B_WHITE "[t] ");
    vkprintf(format, arg);
    kprint("\n" CON_RESET);

    va_end(arg);
}

void info(const char* format, ...) {
    last_level = LL_INFO;

    if (~log_mask & LL_INFO)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint(COLOR_B_BLUE "[i] ");
    vkprintf(format, arg);
    kprint("\n" CON_RESET);

    va_end(arg);
}

void warn(const char* format, ...) {
    last_level = LL_WARN;

    if (~log_mask & LL_WARN)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint(COLOR_B_YELLOW "[w] ");
    vkprintf(format, arg);
    kprint("\n" CON_RESET);

    va_end(arg);
}

void error(const char* format, ...) {
    last_level = LL_ERROR;
    
    if (~log_mask & LL_ERROR)
        return;

    va_list arg;
    va_start(arg, format);
    
    kprint(COLOR_B_RED "[e] ");
    vkprintf(format, arg);
    kprint("\n" CON_RESET);

    va_end(arg);
}

void append(const char* format, ...) {
    if (~log_mask & last_level)
        return;

    va_list arg;
    va_start(arg, format);
    
    vkprintf(format, arg);
    kprint("\n");

    va_end(arg);
}