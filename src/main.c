#include "asmutils.h"
#include "types.h"

#define DISPLAY ((u16*) 0xb8000)
#define DEF_ATTR 0x1f

#define SCR_W 80
#define SCR_H 25
#define SCR_SZ (SCR_W * SCR_H)

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

void kmain(void)
{
        cls();
        putstr(29, 12, "welcome to rogue os \01");

        endless_loop();
}
