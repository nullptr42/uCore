#pragma once

namespace cxx {

#include <stdarg.h>
#include <stddef.h>

int vsnprintf(char *buffer, size_t size, const char *format, va_list arg);
int snprintf(char *buffer, size_t size, const char *format, ...);

void putc(char c);
void puts(const char *str);
int printf(const char *format, ...);
int vprintf(const char *format, va_list arg);

} // namespace cxx