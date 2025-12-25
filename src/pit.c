#include "pit.h"
#include "8259.h"
#include "interrupts.h"

#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND 0x43

#define PIT_BASE_FREQUENCY 1193182

static u32 pit_ticks;
static u32 pit_freq;

// maps to IRQ0, 0x20 currently
static void pit_handler(const struct interrupt_ctx* ctx)
{
        pit_ticks--;
}

/// requires init_interrupts and init_8259
void init_pit(u32 freq)
{
        pit_freq = freq;
        u16 divisor = PIT_BASE_FREQUENCY / freq;

        // 00 - channel 0
        // 11 - Lbyte/Hbyte access
        // 011 - square wave generator mode
        // 0 - 16 bit binary mode
        outb(PIT_COMMAND, 0x36); // 0b00110110

        u8 low = divisor & 0xff;
        u8 high = (divisor >> 8) & 0xff;

        outb(PIT_CHANNEL0_DATA, low);
        outb(PIT_CHANNEL0_DATA, high);

        override_interrupt_handler(0x20, pit_handler, INTERRUPT_GATE);
}

void sleep(u32 ms)
{
        pit_ticks = (ms * pit_freq) / 1000;
        while (pit_ticks) {
                __asm__ volatile("hlt\n");
        }
}
