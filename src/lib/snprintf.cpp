
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

/* Reference: http://www.cplusplus.com/reference/cstdio/printf/ */

enum class DataType {
    None,
    Char,
    ShortInteger,
    LongInteger,
    LongLongInteger,
    IntMax,
    Size,
    PointerDifference,
    LongDouble,
    Pointer  
};

struct FormatState {
    char pad_chr;
    int  pad_len;
    bool justify_left;
    bool show_plus;
    bool pound;
    int precision;
    DataType data_type;

    int i;
    size_t total;
    size_t left;
    const char* format;
    char* buffer;
};

inline bool is_numeric_char(char character) {
    return character >= '0' && character <= '9';
}

inline int parse_number(FormatState& state) {
    auto format = state.format;
    auto result = format[state.i++] - '0';
    while (format[state.i] && is_numeric_char(format[state.i])) {
        result *= 10;
        result += format[state.i++] - '0';
    }
    return result;
}

static inline void parse_flags(FormatState& state) {
    const char* format = state.format;
    
    while (1) {
        switch (format[state.i]) {
            case '-':
                state.justify_left = true;
                break;
            case '+':
                state.show_plus = true;
                break;
            case ' ':
                /* TODO */
                break;
            case '#':
                state.pound = true;
                break;
            case '0':
                state.pad_chr = '0';
                break;
            default:
                return;
        }
        state.i++;
    }
}

static inline bool parse_width(FormatState& state, va_list arg) {
    const char* format = state.format;

    if (format[state.i] == 0)
        return false;

    if (is_numeric_char(format[state.i])) {
        state.pad_len = parse_number(state);
    } else if (format[state.i] == '*') {
        state.pad_len = va_arg(arg, int);
        state.i++;
    }

    return true;    
}

static inline bool parse_precision(FormatState& state, va_list arg) {
    const char* format = state.format;

    if (format[state.i] == 0)
        return false;

    if (format[state.i] == '.') {
        state.i++;
        if (format[state.i] == '*') {
            state.precision = va_arg(arg, int);
            state.i++;
        } else if (is_numeric_char(format[state.i])) {
            state.precision = parse_number(state);
        }
    }
    
    return true;
}

static inline bool parse_length(FormatState& state) {
    const char* format = state.format;

    if (format[state.i] == 0)
        return false;
    
    switch (format[state.i]) {
        case 'h':
            if (format[state.i + 1] == 'h') {
                state.data_type = DataType::Char;
                state.i += 2;
            } else {
                state.data_type = DataType::ShortInteger;   
                state.i++;
            }
            break;
        case 'l':
            if (format[state.i + 1] == 'l') {
                state.data_type = DataType::LongLongInteger;
                state.i += 2;
            } else {
                state.data_type = DataType::LongInteger;
                state.i++;
            }
            break;
        case 'j':
            state.data_type = DataType::IntMax;
            state.i++;
            break;
        case 'z':
            state.data_type = DataType::Size;
            state.i++;
            break;
        case 't':
            state.data_type = DataType::PointerDifference;
            state.i++;
            break;
        case 'L':
            state.data_type = DataType::LongDouble;
            state.i++;
            break;
    }
    
    return true;
}

static int get_left_pad(FormatState& state) {
    return state.justify_left ? 0 : state.pad_len;
}

static int get_right_pad(FormatState& state) {
    return state.justify_left ? state.pad_len : 0;
}

static inline void append(FormatState& state, char chr) {
    state.total++;
    if (state.left > 0) {
        state.buffer[0] = chr;
        state.buffer++;
        state.left--;
    }
}

static inline void append_string(FormatState& state, const char* string) {
    int i = 0;
    int padding = get_right_pad(state);

    while (string[i])
        append(state, string[i++]);
    while (i++ < padding)
        append(state, ' ');
}

static inline bool fmt_signed_int(FormatState& state, va_list arg) {
    int digits = 0;
    int padding = get_left_pad(state);
    intmax_t value;

    /* Get number to print */
    switch (state.data_type) {
        case DataType::None:
        case DataType::Char:
        case DataType::ShortInteger:
            value = va_arg(arg, int);
            break;
        case DataType::LongLongInteger:
            value = va_arg(arg, long long);
            break;
        case DataType::LongInteger:
            value = va_arg(arg, long);
            break;
        case DataType::IntMax:
            value = va_arg(arg, intmax_t);
            break;
        case DataType::Size: /* checkme */
            value = va_arg(arg, size_t);
            break;
        case DataType::PointerDifference: /* checkme */
            value = va_arg(arg, ptrdiff_t);
            break;
        default:
            return false; 
    }

    intmax_t temp = value;
    bool show_sign = value < 0 || state.show_plus;

    /* Count number of digits */
    while (temp) { digits++; temp /= 10; }
    
    if (digits == 0)
        digits = 1;

    /* "For integer specifiers (d, i, o, u, x, X):
     *  precision specifies the minimum number of digits to be written."
     */
    if (state.precision > digits)
        digits = state.precision;

    int size = digits;

    /* Determine required string length */
    if (show_sign)
        size++;
    if (padding > size)
        size = padding;

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
        buffer[i] = state.pad_chr;

    /* Place sign character */
    if (show_sign) {
        char sign = (value < 0) ? '-' : '+';

        if (state.pad_chr == ' ')
            buffer[digits_start-1] = sign;
        else
            buffer[0] = sign;
    }

    append_string(state, buffer);
    return true;
}

static inline bool fmt_unsigned_int(FormatState& state,
                                    int base,
                                    const char* alphabet,
                                    const char* prefix,
                                    va_list arg) {
    int digits = 0;
    int padding = get_left_pad(state);
    uintmax_t value;

    /* Get number to print */
    switch (state.data_type) {
        case DataType::None:
        case DataType::Char:
        case DataType::ShortInteger:
            value = va_arg(arg, unsigned int);
            break;
        case DataType::LongInteger:
            value = va_arg(arg, unsigned long int);
            break;
        case DataType::LongLongInteger:
            value = va_arg(arg, unsigned long long int);
            break;
        case DataType::IntMax:
            value = va_arg(arg, uintmax_t);
            break;
        case DataType::Size:
            value = va_arg(arg, size_t);
            break;
        case DataType::PointerDifference:
            value = va_arg(arg, ptrdiff_t);
            break;
        case DataType::Pointer:
            value = (uintmax_t)va_arg(arg, void*);
            break;
        default:
            return false;
    }

    int prefix_length = 0;

    /* Determine prefix length if needed */
    if (state.pound) {
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
    if (state.precision > digits)
        digits = state.precision;

    int size = digits + prefix_length;

    /* Determine required string length */
    if (padding > size)
        size = padding;

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
        buffer[i] = state.pad_chr;

    /* Print number system prefix (e.g. base 16 0x) */
    if (state.pound) {
        int start = (state.pad_chr == '0') ? 0 : (digits_start - prefix_length);
        for (int i = 0; i < prefix_length; i++)
            buffer[start + i] = prefix[i];
    }

    append_string(state, buffer);
    return true;
}

static inline bool fmt_char(FormatState& state, va_list arg) {
    char chr = va_arg(arg, int);

    int padding = get_left_pad(state);
    int size = (padding > 1) ? padding : 1;
    char buffer[size+1];

    /* TODO: GNU printf does ignore '0'-pad with %c.
     * But is this standard? */
    for (int i = 0; i < size - 1; i++)
        buffer[i] = ' ';
    
    buffer[size - 1] = chr;
    buffer[size] = '\0';
    
    append_string(state, buffer);
    return true;
}

static inline bool fmt_string(FormatState& state, va_list arg) {
    const char* string = va_arg(arg, char*);

    int len = 0;
    int pad = get_left_pad(state);

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

static inline bool do_format(FormatState& state, va_list arg) {
    switch (state.format[state.i]) {
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
            *(va_arg(arg, int*)) = (int)state.total;
            return true;
        case 'p':
            state.pound = true;
            state.data_type = DataType::Pointer;
            return fmt_unsigned_int(state, 16, "0123456789abcdef", "0x", arg);
    }
    
    return false;
}

static inline void init_params(FormatState& state) {
    state.pad_chr = ' ';
    state.pad_len = 0;
    state.justify_left = false;
    state.show_plus = false;
    state.pound = false;
    state.precision = -1;
    state.data_type = DataType::None;
}

int vsnprintf(char* buffer, size_t size, const char* format, va_list arg) {
    /* Maintain information such as formatting flags and buffer information */
    FormatState state;

    state.buffer = buffer;
    state.i = 0;
    state.total = 0;
    state.left = size;
    state.format = format;

    /* Account for null-byte terminator */
    if (size > 0) state.left--;

    /* Iterate format string */
    while (format[state.i]) {
        if (format[state.i] == '%') {
            state.i++;

            /* Special case: %% will print just % */
            if (format[state.i] == '%') {
                append(state, '%');
                state.i++;
                continue;
            }

            /* Reset parameters for format specifier */
            init_params(state);

            /* Parse options for format specifier */
            parse_flags(state);
            if (!parse_width(state, arg))
                return state.total * -1;
            if (!parse_precision(state, arg))
                return state.total * -1;
            if (!parse_length(state))
                return state.total * -1;

            /* Apparently glibc ignores '0'-padding when precision is defined.
             * TODO: check if this is expected / standard behaviour. */
            if (state.pad_chr == '0' && state.precision > 0)
                state.pad_chr = ' ';

            /* Finally perform the formatting operation */
            if (!do_format(state, arg))
                return state.total * -1;            
        } else {
            append(state, format[state.i]);
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
