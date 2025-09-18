bits 16
org 0x7e00

%define REST_SIZE 0x20
%define NJMP SIZE * 1024 / 2 - REST_SIZE

times NJMP db 0xEB, 0x00

  mov si, msg
  mov ah, 0x0e
loop:
  lodsb
  test al, al
  jz fin
  int 0x10
  jmp loop
fin:
  hlt

msg: db "payload loaded!", 0
