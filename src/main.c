#include "8259.h"
#include "asmutils.h"
#include "interrupts.h"
#include "screen.h"

enum special_keys { L_CTRL, L_SHIFT, R_SHIFT, L_ALT, CAPSLOCK };

// clang-format off
const char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', L_CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', L_SHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/', R_SHIFT, '*', L_ALT, ' ', CAPSLOCK,
    // TODO
};
// clang-format on

void better_keyboard_handler(const struct interrupt_ctx* ctx)
{
        u8 scancode = inb(PS2_IN);
        if (!(scancode & 0x80)) {
                printf("%c", scancode_map[scancode]);
        }
        return;
}

static void delay()
{
        for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 20000; j++) {
                        outb(0x80, 0x80);
                }
                printf("%d ", i);
        }
        printf("\n");
}

u16 global = 0;

void timer_handler(const struct interrupt_ctx* ctx)
{
        // printf("$20 to 89134809175 to disable\n");
        set_device(DM_TIMER, false);
        // printf("%d ", global++);
        delay();
        // printf("ended delay");
        // global = 0;
        // if (global < 123) {
        // eoi();
        __asm__ volatile("sti\n");
        delay();
        // }
        // endless_loop();
        return;
}

void keyboard_handler(const struct interrupt_ctx* ctx)
{
        printf("%c", inb(PS2_IN));
        // __asm__ volatile("sti\n");
        endless_loop();
        return;
}

void kmain(void)
{
        init_printer();
        init_interrupts();

        // experiment
        init_8259(true, DM_KEYBOARD | DM_TIMER);
        enum gate_type gate = INTERRUPT_GATE;
        override_interrupt_handler(0x20, timer_handler, gate);
        override_interrupt_handler(0x21, better_keyboard_handler, gate);
        __asm__ volatile("sti\n");
        // for (;;) {
        //         printf("%d ", global++);
        // }
        endless_loop();
}
