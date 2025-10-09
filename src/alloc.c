#include "assert.h"
#include "mem.h"
#include "panic.h"
#include "types.h"

#define ASTART 0x100000
#define AEND 0x400000

static u8* next_free = (u8*) ASTART;

void* imalloc(size_t size, size_t alignment)
{
        assert(size > 0);
        assert(alignment > 0 && !((alignment & (alignment - 1))));

        u8* aligned =
            (u8*) ((size_t) (next_free + alignment - 1) & (~(alignment - 1)));

        if ((size_t) aligned + size > AEND) {
                panic("Out of memory: trying to allocate %d bytes at %p",
                      size,
                      aligned);
                return nil;
        }

        next_free = aligned + size;
        return (void*) aligned;
}

void* icalloc(size_t size, size_t alignment)
{
        void* data = imalloc(size, alignment);
        memzero(data, size);
        return data;
}
