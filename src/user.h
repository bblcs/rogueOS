#pragma once

#include "interrupts.h"

#pragma pack(push, 1)
struct uctx {
        struct interrupt_ctx ctx;
        u32 esp;
        u16 ss;
        u16 _av;
};
#pragma pack(pop)

void start_uproc(void* entry, void* stack);
