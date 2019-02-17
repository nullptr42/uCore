/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <lib/libc/stdio.h>
#include <lib/vt100/vt100.h>
#include <arch/x86_64/io-port.h>

/* VGA hardware ports */
enum vga_ports {
    VGA_PORT_REG  = 0x3D4,
    VGA_PORT_DATA = 0x3D5
};

/* VGA registers */
enum vga_registers {
    VGA_REG_CURSOR_HI = 14,
    VGA_REG_CURSOR_LO = 15
};

/**
 * @brief VGA-compatible text mode colors.
 */
enum console_color {
    CONSOLE_BLACK    = 0,
    CONSOLE_BLUE     = 1,
    CONSOLE_GREEN    = 2,
    CONSOLE_CYAN     = 3,
    CONSOLE_RED      = 4,
    CONSOLE_MAGENTA  = 5,
    CONSOLE_BROWN    = 6,
    CONSOLE_LGRAY    = 7,
    CONSOLE_DGRAY    = 8,
    CONSOLE_LBLUE    = 9,
    CONSOLE_LGREEN   = 10,
    CONSOLE_LCYAN    = 11,
    CONSOLE_LRED     = 12,
    CONSOLE_LMAGENTA = 13,
    CONSOLE_YELLOW   = 14,
    CONSOLE_WHITE    = 15,
};

static const int terminal_width  = 80;
static const int terminal_height = 25;
static const int qwords_per_line = 20;
static const int qwords_per_page = 500;

static const uint64_t qword_VT100_COLOR_FG_mul = 0x0100010001000100;
static const uint64_t qword_spaces = 0x0020002000200020;

static inline bool is_invalid_coordinate(int x, int y) {
    return x < 0 || x >= terminal_width ||
           y < 0 || y >= terminal_height;
}

static inline uint16_t get_address(int x, int y) {
    return y * terminal_width + x;
}

/* Color Byte
 * MSN: Background Color
 * LSN: Foreground Color
 */ 
static uint8_t vga_color = 0x07;

/* Map VT100 color to i386 VGA textmode color. */
static const uint8_t VT100_COLOR_FG_map[16] = {
    CONSOLE_BLACK, CONSOLE_RED     , CONSOLE_GREEN , CONSOLE_BROWN,
    CONSOLE_BLUE , CONSOLE_MAGENTA , CONSOLE_CYAN  , CONSOLE_LGRAY,
    CONSOLE_DGRAY, CONSOLE_LRED    , CONSOLE_LGREEN, CONSOLE_YELLOW,
    CONSOLE_LBLUE, CONSOLE_LMAGENTA, CONSOLE_LCYAN , CONSOLE_WHITE
};

/* VGA buffer */
static uint16_t* buffer = (uint16_t*)0xFFFFFFFF800B8000;

/* Debug */
void serial_init();
void serial_debug(const char* string);

static void set_cursor(int x, int y) {
    if (is_invalid_coordinate(x, y))
        return;

    uint16_t address = get_address(x, y);

    /* high byte */
    outb(VGA_PORT_REG, VGA_REG_CURSOR_HI);
    outb(VGA_PORT_DATA, address >> 8);
        
    /* low byte */
    outb(VGA_PORT_REG, VGA_REG_CURSOR_LO);
    outb(VGA_PORT_DATA, address & 0xFF);
}

static void set_char(char c, int x, int y) {
    if (is_invalid_coordinate(x, y))
        return;
    buffer[get_address(x, y)] = ((uint16_t)vga_color<<8)|c;
}

static void set_fg_color(enum vt100_color color) {
    vga_color = (vga_color&0xF0)|VT100_COLOR_FG_map[color];
}

static void set_bg_color(enum vt100_color color) {
    vga_color = (vga_color&0x0F)|(VT100_COLOR_FG_map[color]<<4);
}

static void scroll() {
    int count = qwords_per_page - qwords_per_line;
    
    uint64_t* dst = (void*)buffer;
    uint64_t* src = (void*)(buffer + terminal_width);

    /* Move everything one line up */
    for (int i = 0; i < count; i++)
        dst[i] = src[i];

    /* Clear last line */
    dst += count;
    for (int i = 0; i < qwords_per_line; i++)
        dst[i] = (vga_color*qword_VT100_COLOR_FG_mul)|qword_spaces;
}

/* VT100 glue */
struct vt100_term term;
struct vt100_driver driver = {
    .set_cursor   = set_cursor,
    .set_char     = set_char,
    .set_fg_color = set_fg_color,
    .set_bg_color = set_bg_color,
    .scroll       = scroll
};

void print_init() {
    uint64_t* _buffer = (void*)buffer;

    /* Clear screen */
    for (int i = 0; i < qwords_per_page; i++)
        _buffer[i] = (vga_color*qword_VT100_COLOR_FG_mul)|qword_spaces;

    /* Setup serial debug driver */
    serial_init();

    /* Setup VT100 driver */
    driver.width  = terminal_width;
    driver.height = terminal_height;

    /* Setup VT100 terminal */
    vt100_init(&term, &driver);
}


/* TODO: place these functions somewhere else */

void kputc(char c) {
    vt100_write(&term, c);
    vt100_update_cursor(&term);
}

void kprint(const char* str) {
    char c;
    serial_debug(str);
    while ((c = *str++)) {
        vt100_write(&term, c);
    }
    vt100_update_cursor(&term);
}

int kprintf(const char* format, ...) {
    va_list arg1, arg2;
    
    va_start(arg1, format);
    va_copy(arg2, arg1);
    
    int total = vsnprintf(NULL, 0, format, arg2);

    if (total > -1) {
        char buffer[total+1];

        vsnprintf(buffer, total+1, format, arg1);
        kprint(buffer);
    }    

    va_end(arg2);    
    va_end(arg1);
    return total;
}

int vkprintf(const char* format, va_list arg) {
    va_list arg2;
    va_copy(arg2, arg);
    
    int total = vsnprintf(NULL, 0, format, arg);

    if (total > -1) {
        char buffer[total+1];

        vsnprintf(buffer, total+1, format, arg2);
        kprint(buffer);
    }

    va_end(arg2);
    return total;
}