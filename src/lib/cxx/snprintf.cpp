/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/* Reference: http://www.cplusplus.com/reference/cstdio/printf/ */

namespace cxx {

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
  int i = 0;
  size_t total = 0;

  char *buffer;       /* output buffer */
  size_t left;        /* remaining space */
  const char *format; /* format string */

  struct Parameters {
    char pad_char = ' ';
    int pad_length = 0;

    bool justify_left = false;
    bool show_plus = false;
    bool pound = false;
    int precision = -1;

    DataType data_type = DataType::None;
  } parameters;

  FormatState(char *buffer, size_t size, const char *format)
      : buffer(buffer), left(size), format(format) {}
};

inline bool is_numeric_char(char character) {
  return character >= '0' && character <= '9';
}

inline int parse_number(FormatState &state) {
  auto format = state.format;
  auto result = format[state.i++] - '0';
  while (format[state.i] && is_numeric_char(format[state.i])) {
    result *= 10;
    result += format[state.i++] - '0';
  }
  return result;
}

static inline void parse_flags(FormatState &state) {
  auto format = state.format;
  auto &params = state.parameters;

  while (1) {
    switch (format[state.i]) {
    case '-': {
      params.justify_left = true;
      break;
    }
    case '+': {
      params.show_plus = true;
      break;
    }
    case ' ': {
      /* TODO */
      break;
    }
    case '#': {
      params.pound = true;
      break;
    }
    case '0': {
      params.pad_char = '0';
      break;
    }
    default: { return; }
    }
    state.i++;
  }
}

static inline bool parse_width(FormatState &state, va_list arg) {
  auto format = state.format;
  auto &params = state.parameters;

  if (format[state.i] == 0)
    return false;

  if (is_numeric_char(format[state.i])) {
    params.pad_length = parse_number(state);
  } else if (format[state.i] == '*') {
    params.pad_length = va_arg(arg, int);
    state.i++;
  }

  return true;
}

static inline bool parse_precision(FormatState &state, va_list arg) {
  auto format = state.format;
  auto &params = state.parameters;

  if (format[state.i] == 0)
    return false;

  if (format[state.i] == '.') {
    state.i++;
    if (format[state.i] == '*') {
      params.precision = va_arg(arg, int);
      state.i++;
    } else if (is_numeric_char(format[state.i])) {
      params.precision = parse_number(state);
    }
  }

  return true;
}

static inline bool parse_length(FormatState &state) {
  auto format = state.format;
  auto &params = state.parameters;

  if (format[state.i] == 0)
    return false;

  switch (format[state.i]) {
  case 'h': {
    if (format[state.i + 1] == 'h') {
      params.data_type = DataType::Char;
      state.i += 2;
    } else {
      params.data_type = DataType::ShortInteger;
      state.i++;
    }
    break;
  }
  case 'l': {
    if (format[state.i + 1] == 'l') {
      params.data_type = DataType::LongLongInteger;
      state.i += 2;
    } else {
      params.data_type = DataType::LongInteger;
      state.i++;
    }
    break;
  }
  case 'j': {
    params.data_type = DataType::IntMax;
    state.i++;
    break;
  }
  case 'z': {
    params.data_type = DataType::Size;
    state.i++;
    break;
  }
  case 't': {
    params.data_type = DataType::PointerDifference;
    state.i++;
    break;
  }
  case 'L': {
    params.data_type = DataType::LongDouble;
    state.i++;
    break;
  }
  }

  return true;
}

static int get_left_pad(FormatState &state) {
  return state.parameters.justify_left ? 0 : state.parameters.pad_length;
}

static int get_right_pad(FormatState &state) {
  return state.parameters.justify_left ? state.parameters.pad_length : 0;
}

static inline void append(FormatState &state, char chr) {
  state.total++;
  if (state.left > 0) {
    state.buffer[0] = chr;
    state.buffer++;
    state.left--;
  }
}

static inline void append_string(FormatState &state, const char *string) {
  int i = 0;
  int padding = get_right_pad(state);

  while (string[i])
    append(state, string[i++]);
  while (i++ < padding)
    append(state, ' ');
}

static inline bool fmt_signed_int(FormatState &state, va_list arg) {
  int digits = 0;
  int padding = get_left_pad(state);
  intmax_t value;
  auto &params = state.parameters;

  /* Get number to print */
  switch (params.data_type) {
  case DataType::None:
  case DataType::Char:
  case DataType::ShortInteger: {
    value = va_arg(arg, int);
    break;
  }
  case DataType::LongLongInteger: {
    value = va_arg(arg, long long);
    break;
  }
  case DataType::LongInteger: {
    value = va_arg(arg, long);
    break;
  }
  case DataType::IntMax: {
    value = va_arg(arg, intmax_t);
    break;
  }
  case DataType::Size: {
    value = va_arg(arg, size_t);
    break;
  }
  case DataType::PointerDifference: {
    value = va_arg(arg, ptrdiff_t);
    break;
  }
  default: { return false; }
  }

  intmax_t temp = value;
  bool show_sign = value < 0 || params.show_plus;

  /* Count number of digits */
  while (temp) {
    digits++;
    temp /= 10;
  }

  if (digits == 0)
    digits = 1;

  /* "For integer specifiers (d, i, o, u, x, X):
   *  precision specifies the minimum number of digits to be written."
   */
  if (params.precision > digits)
    digits = params.precision;

  int size = digits;

  /* Determine required string length */
  if (show_sign)
    size++;
  if (padding > size)
    size = padding;

  char buffer[size + 1];
  int digits_start = size - digits;

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
    buffer[i] = params.pad_char;

  /* Place sign character */
  if (show_sign) {
    char sign = (value < 0) ? '-' : '+';

    if (params.pad_char == ' ')
      buffer[digits_start - 1] = sign;
    else
      buffer[0] = sign;
  }

  append_string(state, buffer);
  return true;
}

static inline bool fmt_unsigned_int(FormatState &state, int base,
                                    const char *alphabet, const char *prefix,
                                    va_list arg) {
  int digits = 0;
  int padding = get_left_pad(state);
  uintmax_t value;
  auto &params = state.parameters;

  /* Get number to print */
  switch (params.data_type) {
  case DataType::None:
  case DataType::Char:
  case DataType::ShortInteger: {
    value = va_arg(arg, unsigned int);
    break;
  }
  case DataType::LongInteger: {
    value = va_arg(arg, unsigned long int);
    break;
  }
  case DataType::LongLongInteger: {
    value = va_arg(arg, unsigned long long int);
    break;
  }
  case DataType::IntMax: {
    value = va_arg(arg, uintmax_t);
    break;
  }
  case DataType::Size: {
    value = va_arg(arg, size_t);
    break;
  }
  case DataType::PointerDifference: {
    value = va_arg(arg, ptrdiff_t);
    break;
  }
  case DataType::Pointer: {
    value = (uintmax_t)va_arg(arg, void *);
    break;
  }
  default: { return false; }
  }

  int prefix_length = 0;

  /* Determine prefix length if needed */
  if (params.pound) {
    /* TODO: implement strlen */
    while (prefix[prefix_length])
      prefix_length++;
  }

  uintmax_t temp = value;

  /* Count number of digits */
  while (temp) {
    digits++;
    temp /= base;
  }

  if (digits == 0)
    digits = 1;

  /* "For integer specifiers (d, i, o, u, x, X):
   *  precision specifies the minimum number of digits to be written."
   */
  if (params.precision > digits)
    digits = params.precision;

  int size = digits + prefix_length;

  /* Determine required string length */
  if (padding > size)
    size = padding;

  char buffer[size + 1];
  int digits_start = size - digits;

  /* Print all digits */
  temp = value;
  for (int i = digits - 1; i >= 0; i--) {
    buffer[digits_start + i] = alphabet[temp % base];
    temp /= base;
  }
  buffer[size] = '\0';

  /* Pad string to width */
  for (int i = 0; i < digits_start; i++)
    buffer[i] = params.pad_char;

  /* Print number system prefix (e.g. base 16 0x) */
  if (params.pound) {
    int start = (params.pad_char == '0') ? 0 : (digits_start - prefix_length);
    for (int i = 0; i < prefix_length; i++)
      buffer[start + i] = prefix[i];
  }

  append_string(state, buffer);
  return true;
}

static inline bool fmt_char(FormatState &state, va_list arg) {
  char chr = va_arg(arg, int);

  int padding = get_left_pad(state);
  int size = (padding > 1) ? padding : 1;
  char buffer[size + 1];

  /* TODO: GNU printf does ignore '0'-pad with %c.
   * But is this standard? */
  for (int i = 0; i < size - 1; i++)
    buffer[i] = ' ';

  buffer[size - 1] = chr;
  buffer[size] = '\0';

  append_string(state, buffer);
  return true;
}

static inline bool fmt_string(FormatState &state, va_list arg) {
  const char *string = va_arg(arg, char *);

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

static inline bool do_format(FormatState &state, va_list arg) {
  auto &params = state.parameters;

  switch (state.format[state.i]) {
  /* Signed decimal */
  case 'd':
  case 'i': {
    return fmt_signed_int(state, arg);
  }
  /* Unsigned integer */
  case 'u': {
    return fmt_unsigned_int(state, 10, "0123456789", "", arg);
  }
  case 'o': {
    return fmt_unsigned_int(state, 8, "01234567", "0", arg);
  }
  case 'x': {
    return fmt_unsigned_int(state, 16, "0123456789abcdef", "0x", arg);
  }
  case 'X': {
    return fmt_unsigned_int(state, 16, "0123456789ABCDEF", "0X", arg);
  }

  /* String & Char */
  case 'c': {
    return fmt_char(state, arg);
  }
  case 's': {
    return fmt_string(state, arg);
  }

  /* Misc. */
  case 'n': {
    *(va_arg(arg, int *)) = (int)state.total;
    return true;
  }
  case 'p': {
    params.pound = true;
    params.data_type = DataType::Pointer;
    return fmt_unsigned_int(state, 16, "0123456789abcdef", "0x", arg);
  }
  }

  return false;
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list arg) {
  /* Maintain information such as formatting flags and buffer information */
  FormatState state{buffer, size, format};

  auto &params = state.parameters;

  /* Account for null-byte terminator */
  if (size > 0)
    state.left--;

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

      state.parameters = {};

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
      if (params.pad_char == '0' && params.precision > 0)
        params.pad_char = ' ';

      /* Finally perform the formatting operation */
      if (!do_format(state, arg))
        return state.total * -1;
    } else {
      append(state, format[state.i]);
    }
    state.i++;
  }

  /* Append null terminator if we have a buffer */
  if (size > 0)
    state.buffer[0] = 0;

  return state.total;
}

int snprintf(char *buffer, size_t size, const char *format, ...) {
  va_list arg;

  va_start(arg, format);
  int total = vsnprintf(buffer, size, format, arg);
  va_end(arg);
  return total;
}

} // namespace cxx