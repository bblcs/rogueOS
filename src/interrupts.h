#pragma once

#include "assert.h"
#include "types.h"

#define SYSCALL 0x30

// clangd goes crazy...
#pragma pack(push, 1)
struct interrupt_ctx {
        u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
        u16 gs, : 16, fs, : 16, es, : 16, ds, : 16;
        u8 vec;
        u8 __avaiable[3];
        u32 errcode;
        u32 eip;
        u16 cs, : 16;
        u32 eflags;
};
#pragma pack(pop)

static_assert(sizeof(struct interrupt_ctx) == 68, size_of_int_ctx);

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
