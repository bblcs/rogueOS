#include "interrupts.h"
#include "alloc.h"
#include "assert.h"
#include "panic.h"
#include "types.h"

#define TRAMP_SZ 8
#define N_VEC 256

#define JMP 0xe9
#define JMP_SZ 0x5
#define PUSHEAX 0x50
#define PUSH 0x6a

extern void collect_ctx();

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

#pragma pack(push, 1)
struct idt_desc {
        u16 off0;
        u16 sel;
        u8 zero;
        u8 type : 3;
        u8 fix01 : 2;
        u8 dpl : 2;
        u8 present : 1;
        u16 off1;
};
#pragma pack(pop)

static_assert(sizeof(struct idt_desc) == 8, size_of_idt_desc);

static bool errprone(u8 vec)
{
        switch (vec) {
        case 0x8:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0x11:
        case 0x15:
        case 0x1d:
        case 0x1e:
                return true;
        default:
                return false;
        }
}

static void filltramp(u8* tramp, u8 vec)
{
        size_t off = 0;

        if (!errprone(vec)) {
                tramp[off++] = PUSHEAX;
        }

        tramp[off++] = PUSH;
        tramp[off++] = vec;
        tramp[off] = JMP;

        u32 collector_off =
            (size_t) collect_ctx - (size_t) (tramp + off + JMP_SZ);

        off++;

        *(u32*) (tramp + off) = collector_off;
}

static void* gentramps()
{
        u8* tramps = imalloc(TRAMP_SZ * N_VEC, 8);

        for (int vec = 0; vec < N_VEC; vec++) {
                filltramp(tramps + vec * TRAMP_SZ, vec);
        }

        return tramps;
}

static void filldesc(struct idt_desc* desc, u8* tramp)
{
        desc->off0 = ((size_t) tramp) & 0xffff;
        desc->off1 = (((size_t) tramp) >> 0x10) & 0xffff;
        desc->sel = 8;     // code
        desc->zero = 0;    // fix + fix as 0
        desc->fix01 = 1;   // fix
        desc->dpl = 0;     // not in this lab (priv stuff)
        desc->present = 1; // yes
        desc->type = 6;    // 0b110 - gate interrupt
}

static struct idt_desc* genidt(u8* tramps)
{
        struct idt_desc* start =
            imalloc(N_VEC * sizeof(struct idt_desc), sizeof(struct idt_desc));

        for (size_t vec = 0; vec < N_VEC; vec++) {
                filldesc(start + vec, tramps + vec * TRAMP_SZ);
        }

        return start;
}

#pragma pack(push, 1)
struct idt_pseudo_descriptor {
        u16 limit;
        struct idt_desc* base;
};
#pragma pack(pop)

void interrupts_setup()
{
        struct idt_pseudo_descriptor p;
        p.limit = N_VEC * sizeof(struct idt_desc) - 1;
        p.base = genidt(gentramps());
        __asm__ volatile("lidt %0" ::"m"(p));
}

void unihandler(struct interrupt_ctx* ctx)
{
        panic("uni #%x at %x:%x\n"
              "    eax: %x\n"
              "    ecx: %x\n"
              "    edx: %x\n"
              "    ebx: %x\n"
              "    ebp: %x\n"
              "    esi: %x\n"
              "    edi: %x\n"
              "   *eip: %x\n"
              "   *esp: %x\n"
              "     ds: %x\n"
              "     es: %x\n"
              "     fs: %x\n"
              "     gs: %x\n"
              "\neflags: %x\n"
              "ecode:  %x",
              ctx->vec,
              ctx->cs,
              ctx->eip,
              ctx->eax,
              ctx->ecx,
              ctx->edx,
              ctx->ebx,
              ctx->ebp,
              ctx->esi,
              ctx->edi,
              ctx->eip,
              ctx->esp,
              ctx->ds,
              ctx->es,
              ctx->fs,
              ctx->gs,
              ctx->eflags,
              ctx->errcode);
}
