
/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdarg.h>
#include "log.h"
#include <lib/libc/stdio.h>
#include <lib/vt100/codes.h>

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
            puts(VT100_COLOR_FG_YELLOW "[w] ");
            break;
        case LL_ERROR:
            puts(VT100_COLOR_FG_RED "[e] ");
            break;
        case LL_DEBUG:
            puts(VT100_COLOR_FG_B_BLACK "[d] ");
            break;
        case LL_TRACE:
            puts(VT100_COLOR_FG_B_BLACK "[t] ");
            break;
        default:
            puts(VT100_COLOR_FG_WHITE);
            break;
    }
    vprintf(format, arg);
    puts("\n" VT100_RESET);
    va_end(arg);
}
