#pragma once

#include "types.h"

struct interrupt_ctx;
typedef void (*interrupt_handler)(const struct interrupt_ctx*);

enum gate_type {
        INTERRUPT_GATE = 6, // 0b110
        TRAP_GATE = 7       // 0b111
};

void init_interrupts();
void override_interrupt_handler(u8 vec,
                                interrupt_handler handler,
                                enum gate_type type);

void unihandler(const struct interrupt_ctx*);
