#pragma once

#include "types.h"
#include <stdarg.h>

void cls();
void init_printer();
void set_attr(u8 attr);
void puts(const char* s);
int printf(const char* format, ...);
int vprintf(const char* fmt, va_list args);
