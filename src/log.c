
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

static int log_mask = LL_TRACE | LL_DEBUG | LL_INFO | LL_WARN | LL_ERROR;

void set_log_mask(int mask) {
    log_mask = mask;
}

int get_log_mask() {
    return log_mask;
}

void klog(enum log_level level, const char* format, ...) {
    va_list arg;

    if (~log_mask & level)
        return;

    va_start(arg, format);
    switch (level) {
        case LL_WARN:
            kprint(COLOR_YELLOW "[w] ");
            break;
        case LL_ERROR:
            kprint(COLOR_RED "[e] ");
            break;
        case LL_DEBUG:
            kprint(COLOR_B_BLACK "[d] ");
            break;
        case LL_TRACE:
            kprint(COLOR_B_BLACK "[t] ");
            break;
        default:
            kprint(COLOR_WHITE);
            break;
    }
    vkprintf(format, arg);
    kprint("\n" CON_RESET);
    va_end(arg);
}
