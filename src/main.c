#include "asmutils.h"
#include "interrupts.h"
#include "screen.h"

void kmain(void)
{

        init_printer();
        interrupts_setup();

        __asm__ volatile("mov eax, 0\n"
                         "mov ecx, 1\n"
                         "mov edx, 2\n"
                         "mov ebx, 3\n"
                         "mov ebp, 4\n"
                         "mov esi, 5\n"
                         "mov edi, 6\n");
        // __asm__ volatile("int 0x58\n"); // exp 1
        // __asm__ volatile("div eax\n"); // exp 2
        __asm__ volatile("sti\n"); // exp 3

        endless_loop();
}
