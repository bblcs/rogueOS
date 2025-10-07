#include "asmutils.h"
#include "types.h"

#define DISPLAY ((u16*) 0xb8000)
#define DEF_ATTR 0x1f

#define SCR_W 80
#define SCR_H 25
#define SCR_SZ (SCR_W * SCR_H)

struct vga_char {
        u8 ch;
        u8 attr;
};

void cls()
{
        for (int i = 0; i < SCR_SZ; i++) {
                DISPLAY[i] = ' ' | DEF_ATTR << 8;
        }
}

void putstr(u8 x, u8 y, const char* s)
{
        u16* d = DISPLAY + y * SCR_W + x;
        while (*s) {
                *d++ = *s++ | DEF_ATTR << 8;
        }
}

inline int getidx(u8 x, u8 y)
{
        return y * SCR_W + x;
}

inline u16 vga_char_to_u16(struct vga_char* c)
{
        return c->ch | c->attr << 8;
}

void putchar(u8 x, u8 y, struct vga_char* c)
{
        u16 idx = getidx(x, y);
        if (idx > SCR_SZ) return;
        DISPLAY[idx] = vga_char_to_u16(c);
}

int fibonacci(u32 n)
{
        if (n <= 1) return 1;
        return fibonacci(n - 1) + fibonacci(n - 2);
}

void animation()
{
        struct vga_char man = {1, DEF_ATTR};
        struct vga_char space = {' ', DEF_ATTR};
        const int diff = 3;
        const int man1y = 12 - diff;
        const int man2y = 12 + diff;
        const int time = 16;

        for (int men = 0; men < SCR_W; men++) {
                for (int x = 0; x < SCR_W - men; x++) {
                        putchar(x, man1y, &man);
                        putchar(SCR_W - x - 1, man2y, &man);
                        putchar((x - 1), man1y, &space);
                        putchar(SCR_W - x, man2y, &space);
                        fibonacci(time);
                }
        }
}

void kmain(void)
{
        cls();
        putstr(30, 12, "welcome to rogue os");
        animation();

        endless_loop();
}
