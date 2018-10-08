/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib.h"
#include <stdint.h>
#include <stdbool.h>

/* Reference: http://www.cplusplus.com/reference/cstdio/printf/ */

#define IS_NUMERIC(c) (c >= '0' && c <= '9')

#define PARSE_NUMBER(result)\
    result = format[state->i++] - '0';\
    while (format[state->i] && IS_NUMERIC(format[state->i])) {\
        result *= 10;\
        result += format[state->i++] - '0';\
    }

enum length {
    NONE,
    CHAR,
    SHORT_INT,
    LONG_INT,
    LONG_LONG_INT,
    INTMAX,
    SIZE,
    PTRDIFF,
    LONG_DOUBLE  
};

struct print_state {
    char pad_chr;
    int  pad_len;
    bool justify_left;
    bool show_plus;
    bool pound;
    int precision;
    enum length length;

    int i;
    size_t total;
    size_t left;
    const char* format;
    char* buffer;
};

static inline bool parse_flags(struct print_state* state) {
    const char* format = state->format;
    
    while (1) {
        switch (format[state->i]) {
            case '-':
                state->justify_left = true;
                break;
            case '+':
                state->show_plus = true;
                break;
            case ' ':
                /* TODO */
                break;
            case '#':
                state->pound = true;
                break;
            case '0':
                state->pad_chr = '0';
                break;
            default:
                goto done;
        }
        state->i++;
    }
done:
    return true;
}

static inline bool parse_width(struct print_state* state, va_list arg) {
    const char* format = state->format;

    if (format[state->i] == 0)
        return false;

    if (IS_NUMERIC(format[state->i])) {
        PARSE_NUMBER(state->pad_len);
        return true;
    }
    if (format[state->i] == '*') {
        state->pad_len = va_arg(arg, int);
        state->i++;
        return true;
    }

    return true;    
}

static inline bool parse_precision(struct print_state* state, va_list arg) {
    const char* format = state->format;

    if (format[state->i] == 0)
        return false;

    if (format[state->i] == '.') {
        state->i++;
        if (format[state->i] == '*') {
            state->precision = va_arg(arg, int);
            state->i++;
            return true;
        }
        if (IS_NUMERIC(format[state->i])) {
            PARSE_NUMBER(state->precision);
            return true;
        }
    }
    
    return true;
}

static inline bool parse_length(struct print_state* state) {
    const char* format = state->format;

    if (format[state->i] == 0)
        return false;
    
    switch (format[state->i]) {
        case 'h':
            if (format[state->i + 1] == 'h') {
                state->length = CHAR;
                state->i += 2;
            } else {
                state->length = SHORT_INT;   
                state->i++;
            }
            break;
        case 'l':
            if (format[state->i + 1] == 'l') {
                state->length = LONG_LONG_INT;
                state->i += 2;
            } else {
                state->length = LONG_INT;
                state->i++;
            }
            break;
        case 'j':
            state->length = INTMAX;
            state->i++;
            break;
        case 'z':
            state->length = SIZE;
            state->i++;
            break;
        case 't':
            state->length = PTRDIFF;
            state->i++;
            break;
        case 'L':
            state->length = LONG_DOUBLE;
            state->i++;
            break;
    }
    return true;
}

static inline void append(struct print_state* state, char chr) {
    state->total++;
    if (state->left > 0) {
        state->buffer[0] = chr;
        state->buffer++;
        state->left--;
    }
}

static inline void append_string(struct print_state* state, const char* string) {
    int i = 0;
    while (string[i])
        append(state, string[i++]);
}

static inline bool fmt_signed_int(struct print_state* state, va_list arg) {
    int digits = 0;
    intmax_t value;

    /* Get number to print */
    switch (state->length) {
        case NONE:
        case CHAR:
        case SHORT_INT:
            value = va_arg(arg, int);
            break;
        case LONG_LONG_INT:
            value = va_arg(arg, long long);
            break;
        case LONG_INT:
            value = va_arg(arg, long);
            break;
        case INTMAX:
            value = va_arg(arg, intmax_t);
            break;
        case SIZE: /* checkme */
            value = va_arg(arg, size_t);
            break;
        case PTRDIFF: /* checkme */
            value = va_arg(arg, ptrdiff_t);
            break;
        default:
            return false; 
    }

    intmax_t temp = value;
    bool show_sign = value < 0 || state->show_plus;

    /* Count number of digits */
    while (temp) { digits++; temp /= 10; }
    
    if (digits == 0)
        digits = 1;

    /* "For integer specifiers (d, i, o, u, x, X):
     *  precision specifies the minimum number of digits to be written."
     */
    if (state->precision > digits)
        digits = state->precision;

    int size = digits;

    /* Determine required string length */
    if (show_sign)
        size++;
    if (state->pad_len > size)
        size = state->pad_len;

    char buffer[size+1];
    int  digits_start = size - digits;

    /* Print all digits */    
    temp = value;
    if (temp < 0)
        temp *= -1;
    for (int i = digits - 1; i >= 0; i--) {
        buffer[digits_start + i] = '0' + (temp % 10);
        temp /= 10;
    }
    buffer[size] = '\0';

    /* Pad string to width */
    for (int i = 0; i < digits_start; i++)
        buffer[i] = state->pad_chr;

    /* Place sign character */
    if (show_sign) {
        char sign = (value < 0) ? '-' : '+';

        if (state->pad_chr == ' ')
            buffer[digits_start-1] = sign;
        else
            buffer[0] = sign;
    }

    append_string(state, buffer);
    return true;
}

static inline bool fmt_unsigned_int(struct print_state* state,
                                    int base,
                                    const char* alphabet,
                                    const char* prefix,
                                    va_list arg) {
    int digits = 0;
    uintmax_t value;

    /* Get number to print */
    switch (state->length) {
        case NONE:
        case CHAR:
        case SHORT_INT:
            value = va_arg(arg, unsigned int);
            break;
        case LONG_INT:
            value = va_arg(arg, unsigned long int);
            break;
        case LONG_LONG_INT:
            value = va_arg(arg, unsigned long long int);
            break;
        case INTMAX:
            value = va_arg(arg, uintmax_t);
            break;
        case SIZE:
            value = va_arg(arg, size_t);
            break;
        case PTRDIFF:
            value = va_arg(arg, ptrdiff_t);
            break;
        default:
            return false;
    }

    int prefix_length = 0;

    /* Determine prefix length if needed */
    if (state->pound) {
        /* TODO: implement strlen */
        while (prefix[prefix_length])
            prefix_length++;
    }

    uintmax_t temp = value;

    /* Count number of digits */
    while (temp) { digits++; temp /= base; }

    if (digits == 0)
        digits = 1;

    /* "For integer specifiers (d, i, o, u, x, X):
     *  precision specifies the minimum number of digits to be written."
     */
    if (state->precision > digits)
        digits = state->precision;

    int size = digits + prefix_length;

    /* Determine required string length */
    if (state->pad_len > size)
        size = state->pad_len;

    char buffer[size+1];
    int  digits_start = size - digits;
        
    /* Print all digits */    
    temp = value;
    for (int i = digits - 1; i >= 0; i--) {
        buffer[digits_start + i] = alphabet[temp % base];
        temp /= base;
    }
    buffer[size] = '\0';        

    /* Pad string to width */
    for (int i = 0; i < digits_start; i++)
        buffer[i] = state->pad_chr;

    /* Print number system prefix (e.g. base 16 0x) */
    if (state->pound) {
        int start = (state->pad_chr == '0') ? 0 : (digits_start - prefix_length);
        for (int i = 0; i < prefix_length; i++)
            buffer[start + i] = prefix[i];
    }

    append_string(state, buffer);
    return true;
}

static inline bool fmt_char(struct print_state* state, va_list arg) {
    char chr = va_arg(arg, int);

    /* TODO: GNU printf does ignore '0'-pad with %c.
     * But is this standard? */
    if (state->pad_len > 1) {
        for (int i = 0; i < state->pad_len - 1; i++)
            append(state, ' '); 
    }
    append(state, chr);
    return true;
}

static inline bool fmt_string(struct print_state* state, va_list arg) {
    char* string = va_arg(arg, char*);

    int len = 0;
    int pad = state->pad_len;

    if (string == NULL)
        string = "(null)";

    while (string[len])
        len++;

    /* TODO: GNU printf does ignore '0'-pad with %s.
         * But is this standard? */
    while (pad > len) {
        append(state, ' ');
        pad--;
    }
    
    append_string(state, string);
    return true;
}

static inline bool do_format(struct print_state* state, va_list arg) {
    switch (state->format[state->i]) {
        /* Signed decimal */
        case 'd':
        case 'i':
            return fmt_signed_int(state, arg);

        /* Unsigned integer */
        case 'u':
            return fmt_unsigned_int(state, 10, "0123456789", "", arg);
        case 'o':
            return fmt_unsigned_int(state, 8, "01234567", "0", arg);
        case 'x':
            return fmt_unsigned_int(state, 16, "0123456789abcdef", "0x", arg);
        case 'X':
            return fmt_unsigned_int(state, 16, "0123456789ABCDEF", "0X", arg);

        /* String & Char */
        case 'c':
            return fmt_char(state, arg);
        case 's':
            return fmt_string(state, arg);

        /* Misc. */
        case 'n':
            *(va_arg(arg, int*)) = (int)state->total;
            return true;
    }
    
    return false;
}

static inline void init_params(struct print_state* state) {
    state->pad_chr = ' ';
    state->pad_len = 0;
    state->justify_left = false;
    state->show_plus = false;
    state->pound = false;
    state->precision = -1;
    state->length = NONE;
}

int vsnprintf(char* buffer, size_t size, const char* format, va_list arg) {
    struct print_state state = {
        .buffer = buffer,
        .i      = 0,
        .total  = 0,
        .left   = size,
        .format = format,
    };

    if (size > 0) state.left--;

    while (format[state.i]) {
        if (format[state.i] == '%') {
            state.i++;

            /* Special case: %% will print just % */
            if (format[state.i] == '%') {
                append(&state, '%');
                state.i++;
                continue;
            }

            /* Reset parameters for format specifier */
            init_params(&state);

            /* Parse options for format specifier */
            if (!parse_flags(&state))
                return state.total * -1;
            if (!parse_width(&state, arg))
                return state.total * -1;
            if (!parse_precision(&state, arg))
                return state.total * -1;
            if (!parse_length(&state))
                return state.total * -1;

            /* Apparently glibc ignores '0'-padding when precision is defined.
             * TODO: check if this is expected / standard behaviour. */
            if (state.pad_chr == '0' && state.precision > 0)
                state.pad_chr = ' ';

            /* Finally perform the formatting operation */
            if (!do_format(&state, arg))
                return state.total * -1;            
        } else {
            append(&state, format[state.i]);
        }
        state.i++;
    }

    /* Append null terminator if we have a buffer */
    if (size > 0) state.buffer[0] = 0;

    return state.total;
}

int snprintf(char* buffer, size_t size, const char* format, ...) {
    va_list arg;
    va_start(arg, format);

    int total = vsnprintf(buffer, size, format, arg);

    va_end(arg);
    return total;
}
