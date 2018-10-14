/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _LOG_H_
#define _LOG_H_

enum log_level {
    LL_TRACE = 1<<0,
    LL_INFO  = 1<<1,
    LL_WARN  = 1<<2,
    LL_ERROR = 1<<3
};

void set_log_mask(int mask);
int get_log_mask();

void trace(const char* format, ...);
void info(const char* format, ...);
void warn(const char* format, ...);
void error(const char* format, ...);

#endif //_LOG_H_