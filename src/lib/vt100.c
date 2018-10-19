/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "vt100.h"
#include <stdint.h>

static inline void vt100_print_char(struct vt100_term* this, char c) {
    struct vt100_driver* driver = this->driver;

    if (this->cursor_x == driver->width) {
        if (this->linewrap) {
            this->cursor_x = 0;
            this->cursor_y++;
        }
        else {
            // Horizontal overflow, do nothing.
            return;
        }
    }
    else if (this->cursor_y == driver->height) {
        driver->scroll();
        this->cursor_x = 0;
        this->cursor_y = driver->height - 1;
    }
    driver->set_char(c, this->cursor_x++, this->cursor_y);
}

static inline void vt100_initial(struct vt100_term* this, char c) {
    switch (c) {
        // Carriage Return (CR)
        case '\r': {
            this->cursor_x = 0;
            break;
        }
        // New Line (NL)
        case '\n': {
            this->cursor_x = 0;
            this->cursor_y++;
            break;
        }
        // Horizontal Tab (TAB)
        case '\t': {
            int spaces;

            /* TODO: rework tab logic */
            if (this->cursor_x != 0) {
                spaces = ((this->cursor_x - 1) & 7) ^ 7;
            } else {
                spaces = 9;
            }
            while (spaces--) {
                vt100_print_char(this, ' ');
            }
            break;
        }
        // Escape Sequence (ESC)
        case '\e': {
            this->state = VT100_ESCAPE;
            break;
        }

        // Not a special character, just print it to the screen.
        default: {
            vt100_print_char(this, c);
            break;
        }
    }
}

static inline void vt100_escape(struct vt100_term* this, char c) {
    switch (c) {
        // CSI - Control Sequence Introducer
        case '[': {
            // Clear any left over CSI data
            this->csi.num = 0;
            for (int i = 0; i < CSI_MAX_PARAMS; i++) {
                this->csi.params[i] = 0;
            }
            this->state = VT100_CONTROL_SEQ;
            break;
        }

        // RIS - Reset to Initial State
        case 'c':
        default: {
            // escape sequence ignored.
            this->state = VT100_INITIAL;
            break;
        }
    }
}

static inline void vt100_control(struct vt100_term* this, char c) {
    struct vt100_driver* driver = this->driver;

    switch (c) {
        // Digit, "append" to current parameter.
        case '0': case '1':
        case '2': case '3':
        case '4': case '5':
        case '6': case '7':
        case '8': case '9': {
            struct vt100_csi* csi = &this->csi;

            if (csi->num >= CSI_MAX_PARAMS) {
                break;
            }
            csi->params[csi->num] *= 10;
            csi->params[csi->num] += c - '0';
            break;
        }

        // Ṕarameter delimiter
        case ';': {
            this->csi.num++;
            break;
        }

        // Set Display Attributes
        case 'm': {
            struct vt100_csi* csi = &this->csi;

            for (int i = 0; i <= csi->num; i++) {
                int attribute = csi->params[i];

                switch (attribute) {
                    // Reset state
                    case 0: {
                        // TODO: implement more
                        driver->set_fg_color(VT100_COLOR_WHITE);
                        driver->set_bg_color(VT100_COLOR_BLACK);
                        break;
                    }

                    // Color Brightness
                    case 1: this->intensity = 8; break;
                    case 2: this->intensity = 0; break;

                    // Foreground color
                    case 30: case 31:
                    case 32: case 33:
                    case 34: case 35:
                    case 36: case 37: {
                        driver->set_fg_color(attribute - 30 + this->intensity);
                        break;
                    }

                    // Background color
                    case 40: case 41:
                    case 42: case 43:
                    case 44: case 45:
                    case 46: case 47: {
                        driver->set_bg_color(attribute - 40 + this->intensity);
                        break;
                    }
                }
            }
            this->state = VT100_INITIAL;
            break;
        }

        default: {
            this->state = VT100_INITIAL;
            break;
        }
    }
}

void vt100_init(struct vt100_term* this, struct vt100_driver* driver) {
    this->driver   = driver;
    this->state    = VT100_INITIAL;
    this->cursor_x = 0;
    this->cursor_y = 0;
    this->linewrap = true;
}

void vt100_write(struct vt100_term* this, char c) {
    switch (this->state) {
        case VT100_INITIAL: {
            vt100_initial(this, c);
            break;
        }
        case VT100_ESCAPE: {
            // Last character was 0x1B (aka \e or ESC).
            // This indicates an escape sequence that allows more complex
            // instructions, e.g. setting terminal foreground & background color.
            vt100_escape(this, c);
            break;
        }
        case VT100_CONTROL_SEQ: {
            // Inside of control sequence.
            // Starts optionally with a sequence of arguments/parameters
            // then ends with one character indicating the command.
            vt100_control(this, c);
            break;
        }
    }
}

void vt100_update_cursor(struct vt100_term* this) {
    this->driver->set_cursor(this->cursor_x, this->cursor_y);
}