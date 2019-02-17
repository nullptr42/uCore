/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _K_LIB_VT100_H_
#define _K_LIB_VT100_H_

#include <stdbool.h>

#define CSI_MAX_PARAMS (10)

/**
 * @brief VT1100 foreground/background colors.
 */
enum vt100_color {
    VT100_VT100_COLOR_FG_BLACK   = 0,
    VT100_VT100_COLOR_FG_RED     = 1,
    VT100_VT100_COLOR_FG_GREEN   = 2,
    VT100_VT100_COLOR_FG_YELLOW  = 3,
    VT100_VT100_COLOR_FG_BLUE    = 4,
    VT100_VT100_COLOR_FG_MAGENTA = 5,
    VT100_VT100_COLOR_FG_CYAN    = 6,
    VT100_VT100_COLOR_FG_WHITE   = 7,
};

/**
 * VT100 terminal internal state.
 */
enum vt100_state {
    VT100_INITIAL,
    VT100_ESCAPE,
    VT100_CONTROL_SEQ
};

/**
 * @brief Tell device to set cursor to specific position.
 * @param[in]  x  x-Position
 * @param[in]  y  y-Position
 */
typedef void (*vt100_set_cursor)(int x, int y);

/**
 * @brief Tell device to set character at a specific position.
 * @param[in]  char  c  character
 * @param[in]  int   x  X-position
 * @param[in]  int   y  Y-position
 */
typedef void (*vt100_set_char)(char c, int x, int y);

/**
 * @brief Set background or foreground color on device.
 * @param[in]  console_color  color  VT100-compatible color.
 */
typedef void (*vt100_set_color)(enum vt100_color color);

/**
 * @brief Notify device to remove the top-most line and move everything else up by one line.
 */
typedef void (*vt100_scroll)();

/**
 * @brief VT100 terminal device descriptor.
 */
struct vt100_driver {
    int width;
    int height;

    // driver functions
    vt100_set_cursor set_cursor;
    vt100_set_char   set_char;
    vt100_set_color  set_fg_color;
    vt100_set_color  set_bg_color;
    vt100_scroll     scroll;
};

/**
 * @brief VT100 control sequence parameters.
 */
struct vt100_csi {
    int num;
    int params[CSI_MAX_PARAMS];
};

/**
 * @brief VT100 terminal state.
 */
struct vt100_term {
    struct vt100_driver* driver;

    enum vt100_state state;

    int  cursor_x;
    int  cursor_y;
    bool linewrap;
    int  intensity;

    struct vt100_csi csi;
};

/**
 * @brief Initialize a VT100-like terminal.
 * 
 * @param[in]  struct vt100_term*    this    VT100 terminal to initialize.
 * @param[in]  struct vt100_driver*  driver  Underlying device driver.
 */
void vt100_init(struct vt100_term* this, struct vt100_driver* driver);

/**
 * @brief Write a single character to the VT100 terminal.
 * 
 * @param[in]  struct vt100_term*  this  VT100 terminal to write to.
 * @param[in]  char                c     Character to write.
 */
void vt100_write(struct vt100_term* this, char c);

/**
 * Update the cursor of the underlying driver according to VT100 state.
 */
void vt100_update_cursor(struct vt100_term* this);

#endif // _K_LIB_VT100_H_