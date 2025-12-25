#include "user.h"

extern u32 eflags();
extern void restore_ctx(struct uctx*);

void start_uproc(void* entry, void* stack)
{
        struct uctx uc;
        uc.ctx.cs = 0x1b; // CPL = 3
        uc.ctx.ds = 0x1b; // RPL = 3
        uc.ctx.es = 0x23;
        uc.ctx.fs = 0x23;
        uc.ctx.gs = 0x23;
        uc.ctx.eip = (u32) entry;
        uc.ctx.eflags = (eflags() & (~((u32) 0x3 << 0xc))) |
                        0x1 << 0x9; // IOPL = 0, IF = 1
        uc.ss = 0x23;
        uc.esp = (u32) stack;

        restore_ctx(&uc);
}
