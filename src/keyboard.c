#include "8259.h"
#include "interrupts.h"
#include "types.h"

#define PS2_DATA 0x60
#define PS2_COMMAND 0x64
#define PS2_STATUS 0x64
#define IS_RELEASED 0x80

#define INPUT_BUF_SIZE 0xff

static char input_buf[INPUT_BUF_SIZE];
static u8 input_head = 0;
static u8 input_tail = 0;

enum special_keys {
        L_SHIFT_P = 0x2a,
        L_SHIFT_R = 0xaa,
        R_SHIFT_P = 0x36,
        R_SHIFT_R = 0xb6
};

static const char scancode_to_lowercase[] = {
    0,   0,   '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

static const char scancode_to_uppercase[] = {
    0,   0,   '!',  '@',  '#',  '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{',  '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L',  ':',  '"',  '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M',  '<',  '>',  '?', 0,   '*', 0,   ' '};

static bool shift = false;

inline static u8 next(u32 part)
{
        return (part + 1) % INPUT_BUF_SIZE;
}

static void addchar(u8 c)
{
        input_buf[input_head] = c;
        input_head = next(input_head);
}

static void keyboard_handler(const struct interrupt_ctx* ctx)
{
        unsigned char scancode = inb(0x60);

        if (scancode == L_SHIFT_P || scancode == R_SHIFT_P) {
                shift = 1;
        } else if (scancode == L_SHIFT_R || scancode == R_SHIFT_R) {
                shift = 0;
        } else {
                if (scancode & IS_RELEASED) {
                        // nothing?
                } else {
                        char c;
                        if (shift) {
                                c = scancode_to_uppercase[scancode];
                        } else {
                                c = scancode_to_lowercase[scancode];
                        }
                        if (c) {
                                addchar(c);
                        }
                }
        }
}
void setup_keyboard()
{
        override_interrupt_handler(0x21, keyboard_handler, INTERRUPT_GATE);
}

u8 getch()
{
        while (input_head == input_tail) {
                __asm__ volatile("hlt\n");
        }
        u8 c = input_buf[input_tail];
        input_tail = next(input_tail);

        return c;
}
