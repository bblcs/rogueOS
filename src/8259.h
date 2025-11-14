#pragma once

#include "types.h"

#define UNUSED 0x80
#define MASTER_COMMAND 0x20
#define MASTER_DATA 0x21
#define SLAVE_COMMAND 0xa0
#define SLAVE_DATA 0xa1

// D0 = 1 - we shall set secondary flags
// D1 = 0 - we shall use cascades
// D3 = 0 - we need an edge
// D4 = 1 - we shall start setting 8259 up
// others are for MCS-80/85 (microsoft chip????)
#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01

// icw2 - vector range
#define IRQ_MASTER_START 0x20
#define IRQ_SLAVE_START 0x28

// icw3 - master's pin number for slave
// and bitmask of pins for master
// slave is connected to CAS2
#define CASCADE_PIN 0x02
#define CASCADE_MASK (1 << CASCADE_PIN)

// icw4 - aeoi bit on D1
// D0-4\D1 - for fully nested mode and others
#define ICW4_AUTO 0x02

// device masks
#define DM_NONE 0x00
#define DM_TIMER 0x01
#define DM_KEYBOARD 0x02
#define DM_ALL 0xff

#define PS2_IN 0x60

void init_8259(bool autoeoi, u8 device_mask);
u8 inb(u16 port);
void outb(u16 port, u8 val);
void eoi();
void io_wait();
void set_device(u8 pin, bool on);
