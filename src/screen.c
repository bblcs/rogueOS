#include "screen.h"
#include "mem.h"
#include "types.h"

#include <stdarg.h>

#define DISPLAY ((vga_entry_t*) 0xb8000)
#define DEF_ATTR 0x1f

#define SCR_W 80
#define SCR_H 25
#define SCR_SZ (SCR_W * SCR_H)

typedef u16 vga_entry_t;

static u8 cur_attr = DEF_ATTR;
static u8 cur_x = 0;
static u8 cur_y = 0;

static inline int getidx(u8 x, u8 y)
{
        return y * SCR_W + x;
}

static inline vga_entry_t vga_entry(u8 ch, u8 attr)
{
        return ch | attr << 8;
}

static void rputc_idx(u16 idx, char c)
{
        if (idx > SCR_SZ) return;
        DISPLAY[idx] = vga_entry(c, cur_attr);
}

static void rputc(u8 x, u8 y, char c)
{
        rputc_idx(getidx(x, y), c);
}

static void scroll()
{
        memcpy(DISPLAY,
               DISPLAY + SCR_W,
               (SCR_H - 1) * SCR_W * sizeof(vga_entry_t));

        vga_entry_t blank = vga_entry(' ', cur_attr);

        for (int i = 0; i < SCR_W; i++)
                rputc_idx((SCR_H - 1) * SCR_W + i, blank);
}

static void handle_wrap()
{

        if (cur_x >= SCR_W) {
                cur_x = 0;
                cur_y++;
        }
}

static void handle_scroll()
{

        if (cur_y >= SCR_H) {
                scroll();
                cur_x = 0;
                cur_y = SCR_H - 1;
        }
}

static void smartputc(char c)
{
        if (c == '\n') {
                cur_x = 0;
                cur_y++;
        } else if (c == '\r') {
                cur_x = 0;
        } else {
                rputc(cur_x++, cur_y, vga_entry(c, cur_attr));
        }

        handle_wrap();
        handle_scroll();
}

static void print_itoa(unsigned int n, int base)
{
        char buffer[33];
        int i = 0;

        if (n == 0) {
                smartputc('0');
                return;
        }

        const char* digits = "0123456789abcdef";

        while (n > 0) {
                buffer[i] = digits[n % base];
                n /= base;
                i++;
        }

        while (i-- > 0) {
                smartputc(buffer[i]);
        }
}

static void update_attr()
{
        for (int i = 0; i < SCR_SZ; i++) {
                rputc_idx(i, DISPLAY[i]);
        }
}
void init_printer()
{
        cls();
}
void cls()
{
        for (int i = 0; i < SCR_SZ; i++)
                DISPLAY[i] = ' ' | cur_attr << 8;
        cur_x = 0;
        cur_y = 0;
}

void set_attr(u8 attr)
{
        cur_attr = attr;
        update_attr();
}

void puts(const char* s)
{
        while (*s) {
                smartputc(*s++);
        }
}
int vprintf(const char* fmt, va_list args)
{
        while (*fmt) {
                if (*fmt == '%') {
                        fmt++;
                        switch (*fmt) {
                        case 'c': {
                                u8 c = (u8) va_arg(args, int);
                                smartputc(c);
                                break;
                        }
                        case 's': {
                                char* s = va_arg(args, char*);
                                if (!s) {
                                        s = "(nil)";
                                }
                                puts(s);
                                break;
                        }
                        case 'd': {
                                i32 n = va_arg(args, i32);
                                if (n < 0) {
                                        smartputc('-');
                                        print_itoa(-n, 10);
                                } else {
                                        print_itoa(n, 10);
                                }
                                break;
                        }
                        case 'u': {
                                u32 u = va_arg(args, u32);
                                print_itoa(u, 10);
                                break;
                        }
                        case 'x':
                        case 'p': {
                                u32 u = va_arg(args, u32);
                                puts("0x");
                                print_itoa(u, 16);
                                break;
                        }
                        case '%': {
                                smartputc('%');
                                break;
                        }
                        default: {
                                smartputc('%');
                                smartputc(*fmt);
                                break;
                        }
                        }
                } else {
                        smartputc(*fmt);
                }
                fmt++;
        }

        return 0;
}

int printf(const char* fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        int res = vprintf(fmt, args);
        va_end(args);

        return res;
}
