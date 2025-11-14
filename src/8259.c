#include "8259.h"
#include "types.h"

u8 inb(u16 port)
{
        u8 ret;
        __asm__ volatile("inb %0, %1" : "=a"(ret) : "Nd"(port));
        return ret;
}

static void outb_raw(u16 port, u8 val)
{
        __asm__ volatile("outb %1, %0" ::"a"(val), "Nd"(port));
}

void io_wait()
{
        for (int i = 0; i < 1000; i++) {
                outb_raw(UNUSED, 0);
        }
}

void outb(u16 port, u8 val)
{
        outb_raw(port, val);
        io_wait();
}

void eoi()
{
        outb(MASTER_COMMAND, 0x20);
}

void init_8259(bool autoeoi, u8 device_mask)
{
        // icw1
        outb(MASTER_COMMAND, ICW1_INIT | ICW1_ICW4);
        outb(SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4);

        // icw2
        outb(MASTER_DATA, IRQ_MASTER_START);
        outb(SLAVE_DATA, IRQ_SLAVE_START);

        // icw3
        outb(MASTER_DATA, CASCADE_MASK);
        outb(SLAVE_DATA, CASCADE_PIN);

        // icw4
        outb(MASTER_DATA, ICW4_AUTO * autoeoi);
        outb(SLAVE_DATA, ICW4_AUTO * autoeoi);

        // set masks
        outb(MASTER_DATA, ~device_mask);
        outb(SLAVE_DATA, ~device_mask);
}

void set_device(u8 pin, bool on)
{
        u16 port = MASTER_DATA;
        if (pin > 7) {
                port = SLAVE_DATA;
                pin -= 8;
        }
        u8 cur = ~inb(port);
        u8 dmask = 1 << pin;
        u8 new;
        if (on) {
                new = cur | dmask;
        } else {
                new = cur & ~dmask;
        }

        outb(port, new);
}
