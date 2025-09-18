NASM := nasm -f bin
QEMU := qemu-system-i386 -m 1g -monitor stdio -device VGA


all: clean build run

.tmp/stage1.bin: src/stage1.asm
	$(NASM) -dSIZE=$(SIZE) src/stage1.asm -o .tmp/stage1.bin

.tmp/payload.bin: src/payload.asm
	$(NASM) -dSIZE=$(SIZE) src/payload.asm -o .tmp/payload.bin

boot.img: .tmp/stage1.bin .tmp/payload.bin
	dd if=/dev/zero of=boot.img bs=1024 count=1440
	dd if=.tmp/stage1.bin of=boot.img conv=notrunc
	dd if=.tmp/payload.bin of=boot.img seek=1 conv=notrunc

build: boot.img

clean:
	rm -f *.img
	rm -rf .tmp
	mkdir .tmp

run: build
	$(QEMU) -drive format=raw,file=boot.img,if=floppy

debug: build
	$(QEMU) -drive format=raw,file=boot.img,if=floppy -s -S &
	gdb

.PHONY: all build clean run debug diff
