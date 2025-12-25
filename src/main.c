#include "8259.h"
#include "asmutils.h"
#include "interrupts.h"
#include "keyboard.h"
#include "pit.h"
#include "screen.h"

void kmain(void)
{
        init_printer();
        init_interrupts();
        init_8259(true, DM_TIMER | DM_KEYBOARD);
        init_pit(10000);
        init_keyboard();

        __asm__ volatile("sti\n");

        endless_loop();
}
