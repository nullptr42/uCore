/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <lib/lib.h>
#include <arch/x86_64/io-port.h>

static const int terminal_width  = 80;
static const int terminal_height = 25;

static const int qwords_per_line = 20;
static const int qwords_per_page = 500;

static const uint64_t qword_empty = 0x0720072007200720;

static const int tab_width = 4;

static int terminal_x;
static int terminal_y;

static uint16_t* buffer = (uint16_t*)0xFFFFFFFF800B8000;

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

/* Debug */
void serial_init();
void serial_debug(const char* string);

static inline uint16_t get_address() {
    return terminal_y * terminal_width + terminal_x; 
}

static void update_cursor() {    
    uint16_t address = get_address();

    /* high byte */
    outb(VGA_PORT_REG, VGA_REG_CURSOR_HI);
    outb(VGA_PORT_DATA, address >> 8);

    /* low byte */
    outb(VGA_PORT_REG, VGA_REG_CURSOR_LO);
    outb(VGA_PORT_DATA, address & 0xFF);    
}

void print_init() {
    uint64_t* _buffer = (void*)buffer;

    terminal_x = 0;
    terminal_y = 0;
    for (int i = 0; i < qwords_per_page; i++)
        _buffer[i] = qword_empty;
    serial_init();
}

static void scroll() {
    int count = qwords_per_page - qwords_per_line;
    
    uint64_t* dst = (void*)buffer;
    uint64_t* src = (void*)(buffer + terminal_width);

    for (int i = 0; i < count; i++) {
        dst[i] = src[i];
    }
    
    dst += count;
    for (int i = 0; i < qwords_per_line; i++) {
        dst[i] = qword_empty;
    }
}

static inline void newline() {
    terminal_x = 0;
    terminal_y++;   
}

static inline void print_char(char c) {
    if (terminal_y == terminal_height) {
        terminal_y--;
        scroll();
    }
    
    switch (c) {
        case '\n':
            newline();
            break;
        case '\t':
            terminal_x += tab_width - (terminal_x % tab_width);
            if (terminal_x >= terminal_width) {
                newline();
            }
            break;
        default:
            buffer[get_address()] = 0x0700 | c;
            if (++terminal_x == terminal_width) {
                newline();
            }
            break;
    }
}

void kputc(char c) {
    print_char(c);
    update_cursor();
}

void kprint(const char* str) {
    char c;
    serial_debug(str);
    while ((c = *str++)) {
        print_char(c);
    }
    update_cursor();
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