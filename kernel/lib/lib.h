#ifndef _LIB_H_
#define _LIB_H_

#include <stddef.h>
#include <stdarg.h>

int vsnprintf(char* buffer, size_t size, const char* format, va_list arg);
int snprintf(char* buffer, size_t size, const char* format, ...);

#endif //_LIB_H_