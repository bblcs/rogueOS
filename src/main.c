#include "alloc.h"
#include "asmutils.h"
#include "screen.h"

void kmain(void)
{
        init_printer();

        for (int i = 0;; i++) {
                printf("imalloc returned: %p\n", imalloc(i + 1, 1 << (i % 5)));
        }

        endless_loop();
}
