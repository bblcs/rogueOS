bits 16
org 0x7c00 ; TODO remove when linker script will start working

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

  mov ah, 0x06             ; scroll up
  mov al, 0                ; clear all
  mov bh, 0x1f             ; attribute (magic)  ; 0x09+ are nice
  mov cx, 0                ; (0,0) upper left corner
  mov dl, 79               ; lower right corner x
  mov dh, 24               ; lower right corner y
  int 0x10

  mov ah, 0x02             ; set cursor position
  mov bh, 0                ; page
  mov dl, 0                ; upper left corner x
  mov dh, 0                ; upper left corner y
  int 0x10

  mov ah, 0x01             ; set cursor type
  mov cx, 0h2607           ; blank
  int 0x10

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

  jmp STAGE2   ; use diff after 32 KB, ip overflows

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

times 510-($-$$) db 0
dw 0xaa55
