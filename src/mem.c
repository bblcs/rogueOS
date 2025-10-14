#include "types.h"

void* memmove(void* dst, const void* src, size_t len)
{
        u8* d = (u8*) dst;
        u8* s = (u8*) src;

        if (s < d && s + len > d) {
                s += len;
                d += len;
                while (len--) {
                        *--d = *--s;
                }

        } else {
                while (len--) {
                        *d++ = *s++;
                }
        }

        return dst;
}

void* memcpy(void* dst, const void* src, size_t len)
{
        u8* d = (u8*) dst;
        u8* s = (u8*) src;
        for (size_t i = 0; i < len; i++) {
                d[i] = s[i];
        }

        return dst;
}

void* memzero(void* dst, size_t n)
{
        u8* d = (u8*) dst;
        while (n--)
                *d++ = 0;

        return dst;
}
