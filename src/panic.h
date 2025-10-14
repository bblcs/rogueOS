#pragma once

#include <stdarg.h>

void vpanic(const char* fmt, va_list args);
void panic(const char* fmt, ...);
