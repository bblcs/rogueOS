#include "8259.h"
#include "alloc.h"
#include "asmutils.h"
#include "interrupts.h"
#include "panic.h"
#include "screen.h"
#include "user.h"

extern struct gdt_entry* kernel_code_desc;

struct gdt_entry {
        u16 buh1;
        u16 buh2;
        u8 buh3;
        u8 buh4 : 4;
        u8 buh5 : 1;
        u8 buh6 : 2;
        u8 p : 1;
        u8 buh7;
        u8 buh8;
} __attribute__((packed));

static inline u32 get_esp(void)
{
        u32 val;
        __asm__ volatile("mov %0, esp" : "=r"(val));
        return val;
}

void hardcore(void)
{
        u8 bruh;

        __asm__ volatile("cli\n\t"
                         "in al, dx\n\t"
                         "lgdt [%1]\n\t"
                         "lidt [%1]\n\t"
                         "ltr %1\n\t"
                         "mov cr0, %1"

                         : "=a"(bruh)
                         : "r"(0)
                         : "memory");
}

void gp(const struct interrupt_ctx* ctx)
{
        panic("GP!");
}

u32 global = 1;

void pd(u32 d)
{
        printf("%d ", d);
        if (d == 0) {
                panic("ZERO");
        }
}

void timer_handler(const struct interrupt_ctx* ctx)
{
        // printf("%x", get_esp());
        // endless_loop();
        // global = 0;
        cls();
        return;
}
void syscall_handler(const struct interrupt_ctx* ctx)
{
        printf("%d ", ctx->eax);
}

int syscall(int num)
{
        int res;
        __asm__ volatile("int 0x30" : "=a"(res) : "a"(num) : "memory");
        return res;
}

void stub(const struct interrupt_ctx* ctx)
{
        return;
}

// this code will have kernel level privs
void user_doing_whatever_they_want(const struct interrupt_ctx* ctx)
{
        override_interrupt_handler(0x20, stub, INTERRUPT_GATE);
        printf("handlers still have kernel level privs, and we can overwrite "
               "them from userspace. bye!");
}

void user_program()
{
        // printf("Hello! I am your user! Cherish me!");
        // endless_loop();
        // for (;;) {
        // printf("%d ", global);
        // pd(global);
        // syscall(global);
        // global++;
        // }
        // printf("%x", get_esp());
        // hardcore();
        // endless_loop();

        override_interrupt_handler(
            SYSCALL, user_doing_whatever_they_want, INTERRUPT_GATE);
        syscall(1);
        endless_loop();
}

void kmain(void)
{
        init_printer();
        init_interrupts();
        init_8259(true, DM_TIMER);

        override_interrupt_handler(0xd, gp, INTERRUPT_GATE);
        override_interrupt_handler(0x20, timer_handler, INTERRUPT_GATE);
        override_interrupt_handler(SYSCALL, syscall_handler, INTERRUPT_GATE);

        __asm__ volatile("sti\n");

        void* stack_top = imalloc(8192, 16);
        void* stack_bot = stack_top + 8192;
        start_uproc(user_program, stack_bot);

        endless_loop();
}
