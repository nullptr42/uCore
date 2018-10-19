/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _K_PRINT_VT100_CODES_H_
#define _K_PRINT_VT100_CODES_H_

#define CON_RESET "\x1B[0m"

#define COLOR_BLACK     "\x1B[2;30m"
#define COLOR_RED       "\x1B[2;31m"
#define COLOR_GREEN     "\x1B[2;32m"
#define COLOR_YELLOW    "\x1B[2;33m"
#define COLOR_BLUE      "\x1B[2;34m"
#define COLOR_MAGENTA   "\x1B[2;35m"
#define COLOR_CYAN      "\x1B[2;36m"
#define COLOR_WHITE     "\x1B[2;37m"
#define COLOR_B_BLACK   "\x1B[1;30m"
#define COLOR_B_RED     "\x1B[1;31m"
#define COLOR_B_GREEN   "\x1B[1;32m"
#define COLOR_B_YELLOW  "\x1B[1;33m"
#define COLOR_B_BLUE    "\x1B[1;34m"
#define COLOR_B_MAGENTA "\x1B[1;35m"
#define COLOR_B_CYAN    "\x1B[1;36m"
#define COLOR_B_WHITE   "\x1B[1;37m"

#define BG_COLOR_BLACK     "\x1B[40m"
#define BG_COLOR_RED       "\x1B[41m"
#define BG_COLOR_GREEN     "\x1B[42m"
#define BG_COLOR_YELLOW    "\x1B[43m"
#define BG_COLOR_BLUE      "\x1B[44m"
#define BG_COLOR_MAGENTA   "\x1B[45m"
#define BG_COLOR_CYAN      "\x1B[46m"
#define BG_COLOR_WHITE     "\x1B[47m"

#endif // _K_PRINT_VT100_CODES_H_