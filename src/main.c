#include "asmutils.h"
#include "interrupts.h"
#include "screen.h"

void kmain(void)
{

        init_printer();
        interrupts_setup();

        __asm__ volatile("sti\n");

        endless_loop();
}
