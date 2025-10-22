#include "alloc.h"
#include "asmutils.h"
#include "screen.h"

void kmain(void)
{
        init_printer();

        for (int i = 0;; i++) {
                int size = i + 1;
                int alignment = 1 << (i % 5);
                printf("imalloc(%d, %d) returned: %p\n",
                       size,
                       alignment,
                       imalloc(size, alignment));
        }

        endless_loop();
}
