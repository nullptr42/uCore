
/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdarg.h>
#include <arch/print.h>
#include "log.h"
#include <lib/vt100/vt100-codes.h>

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
            kprint(VT100_COLOR_FG_YELLOW "[w] ");
            break;
        case LL_ERROR:
            kprint(VT100_COLOR_FG_RED "[e] ");
            break;
        case LL_DEBUG:
            kprint(VT100_COLOR_FG_B_BLACK "[d] ");
            break;
        case LL_TRACE:
            kprint(VT100_COLOR_FG_B_BLACK "[t] ");
            break;
        default:
            kprint(VT100_COLOR_FG_WHITE);
            break;
    }
    vkprintf(format, arg);
    kprint("\n" VT100_RESET);
    va_end(arg);
}
