/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _K_LIB_VT100_CODES_H_
#define _K_LIB_VT100_CODES_H_

#define VT100_RESET "\x1B[0m"

#define VT100_COLOR_FG_BLACK     "\x1B[2;30m"
#define VT100_COLOR_FG_RED       "\x1B[2;31m"
#define VT100_COLOR_FG_GREEN     "\x1B[2;32m"
#define VT100_COLOR_FG_YELLOW    "\x1B[2;33m"
#define VT100_COLOR_FG_BLUE      "\x1B[2;34m"
#define VT100_COLOR_FG_MAGENTA   "\x1B[2;35m"
#define VT100_COLOR_FG_CYAN      "\x1B[2;36m"
#define VT100_COLOR_FG_WHITE     "\x1B[2;37m"
#define VT100_COLOR_FG_B_BLACK   "\x1B[1;30m"
#define VT100_COLOR_FG_B_RED     "\x1B[1;31m"
#define VT100_COLOR_FG_B_GREEN   "\x1B[1;32m"
#define VT100_COLOR_FG_B_YELLOW  "\x1B[1;33m"
#define VT100_COLOR_FG_B_BLUE    "\x1B[1;34m"
#define VT100_COLOR_FG_B_MAGENTA "\x1B[1;35m"
#define VT100_COLOR_FG_B_CYAN    "\x1B[1;36m"
#define VT100_COLOR_FG_B_WHITE   "\x1B[1;37m"

#define VT100_COLOR_BG_BLACK     "\x1B[40m"
#define VT100_COLOR_BG_RED       "\x1B[41m"
#define VT100_COLOR_BG_GREEN     "\x1B[42m"
#define VT100_COLOR_BG_YELLOW    "\x1B[43m"
#define VT100_COLOR_BG_BLUE      "\x1B[44m"
#define VT100_COLOR_BG_MAGENTA   "\x1B[45m"
#define VT100_COLOR_BG_CYAN      "\x1B[46m"
#define VT100_COLOR_BG_WHITE     "\x1B[47m"

#endif // _K_LIB_VT100_CODES_H_