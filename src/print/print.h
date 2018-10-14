/*
 * Copyright (C) 2017-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _K_PRINT_H_
#define _K_PRINT_H_

#include <stdarg.h>

/**
  * Callback to a printing device such as e.g. VGA console or serial port.
  * 
  * @param[in]  str  C-styled string to print.
  */ 
typedef void (*pdrv_func_s)(char* str);

/**
  * Callback to a printing device such as e.g. VGA console or serial port.
  * 
  * @param[in]  c  character to print.
  */
typedef void (*pdrv_func_c)(char c);

/**
  * Represents a single printing driver entry.
  */ 
struct print_driver {
    char* name;
    pdrv_func_s print;
    pdrv_func_c print_char;
};

/**
  * Set the list of print device drivers used to output kernel messages.
  * 
  * @param[in]  drivers      list of driver entries
  * @param[in]  num_drivers  number of drivers in the list
  */ 
void print_set_drivers(struct print_driver** drivers, int num_drivers);

/**
 * @brief Print a single character.
 * @param [in] char c character
 */
void print_char(char c);

/**
 * @brief Print a C-styled string.
 * @param [in] char* str C-styled string
 */
void print(char* str);

/**
 * @brief Print a formatted C-style string (va_list version).
 * @param [in] char*   format string template
 * @param [in] va_list args   variable argument list
 * @return 0 = success, -1 = error.
 */
int print_fmt_va(const char* format, va_list args);

/**
 * @brief Print a formatted C-style string.
 * @param [in] char* format string template
 * @return 0 = success, -1 = error.
 */
int print_fmt(const char* format, ...);

#endif // _K_PRINT_H_