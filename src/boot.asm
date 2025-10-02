bits 16

%define NS SIZE * 2
%define STAGE2 0x7e00
%define SECTORS_PER_CYLINDER 18
%define HEADS_PER_CYLINDER 2
%define CYLINDERS 80

  cli

  xor ax, ax
  mov ds, ax
  mov ss, ax
  mov sp, 0x7c00
  mov bx, ax

  mov ah, 0x01             ; set cursor type
  mov cx, 0h2607           ; blank
  int 0x10                 ; can't do that in c?

  mov si, msg
  call print

  mov ax, 0x7e0
  mov es, ax

  mov al, 1                ; 1 sector to read
  mov ch, 0                ; cylinder number
  mov cl, 2                ; sector number
  mov dh, 0                ; head number
  mov dl, 0

  mov si, NS               ; remaining sectors

read_loop:
  mov ah, 0x2              ; 0x2: read sectors from drive
  int 0x13
  jc error

  mov di, es
  add di, 0x20
  mov es, di
  inc cl
  cmp cl, SECTORS_PER_CYLINDER
  jbe .cont
  mov cl, 1
  inc dh
  cmp dh, HEADS_PER_CYLINDER
  jb .cont
  mov dh, 0
  inc ch
  cmp ch, CYLINDERS
  jae error

.cont:
  dec si
  jnz read_loop

  mov si, success_msg
  call print

  lgdt [gdt_desc]
  cld
  mov eax, cr0
  or eax, 1
  mov cr0, eax

  jmp CODE:next

bits 32
next:
  mov eax, DATA
  mov ds, eax
  mov ss, eax
  mov es, eax
  mov fs, eax
  mov gs, eax

  extern kmain
  jmp kmain

  hlt

global endless_loop
endless_loop:
  jmp $

bits 16
error:
  mov si, error_msg
  call print
  hlt

print:
  mov ah, 0x0e
.loop:
  lodsb
  test al, al
  jz .fin
  int 0x10
  jmp .loop
.fin:
  ret


msg: db "welcome to DO-... ah, rogue os!", 0xa, 0xd, 0
success_msg: db "loading stage 2...", 0xa,0xd, 0
error_msg: db "an error occured!", 0xa, 0xd, 0
drive_number: db 0

gdt_desc:
  dw 0x17
  dd gdt_table

align 8
gdt_table:
  .null: dq 0
  code_desc:
    .limit_00_15 dw 0xff
    .base_00_15 dw 0
    .base_23_16 db 0
    .p_dpl_s_type db 0b1001_1010    ; p = 1 : segment present flag
                                    ; dpl = 00 : privilegie level
                                    ; s = 1 : descriptor type flag, normal
                                    ; type = 1(executable)0(nonconformist)1(read)0(acc)
    .g_b_0_avl_limit_16_19: db 0b1100_1111
                                    ; g = 1 : granulate in pages
                                    ; b = 1 : for fun
                                    ; avl = bruh
    .base_24_31 db 0
  data_desc:
    .limit_00_15 dw 0xff
    .base_00_15 dw 0
    .base_23_16 db 0
    .p_dpl_s_type db 0b1001_0010    ; p = 1 : segment present flag
                                    ; dpl = 00 : privilegie level
                                    ; s = 1 : descriptor type flag, normal
                                    ; type = 0(data)0(heresy)1(write)0(acc)
    .g_b_0_avl_limit_16_19: db 0b1100_1111
                                    ; g = 1 : granulate in pages
                                    ; b = 1 : for fun
                                    ; avl = bruh
    .base_24_31 db 0

CODE equ code_desc-gdt_table
DATA equ data_desc-gdt_table

times 510-($-$$) db 0
dw 0xaa55
