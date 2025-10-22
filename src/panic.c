#include "asmutils.h"
#include "screen.h"
#include <stdarg.h>

#define PANIC_ATTR 0xcf

void vpanic(const char* fmt, va_list args)
{
        cli();

        cls();
        set_attr(PANIC_ATTR);
        puts("\01 PANIC!! \01\n\n");
        vprintf(fmt, args);

        endless_loop();
}

void panic(const char* fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        vpanic(fmt, args);
}
