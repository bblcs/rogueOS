.DEFAULT_GOAL := run

CC           := gcc
LD           := ld
NASM         := nasm
QEMU         := qemu-system-i386

SRC_DIR      := src
BUILD_DIR    := build

SOURCES      := $(wildcard $(SRC_DIR)/*.c)
HEADERS      := $(wildcard $(SRC_DIR)/*.h)
BOOT_SRC     := $(SRC_DIR)/boot.asm

KERNEL       := $(BUILD_DIR)/kernel.o
BOOTLOADER   := $(BUILD_DIR)/boot.o

KERNEL_BIN   := $(BUILD_DIR)/kernel.bin
KERNEL_ELF   := $(BUILD_DIR)/kernel.elf

OBJECTS      := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

IMAGE        := $(BUILD_DIR)/os.img

KER_SIZE_KB  := 90

CFLAGS       := -std=c99 -m32 -ffreestanding -fno-pie -no-pie -mno-sse -fno-stack-protector -Wall -Wextra -Os -I$(SRC_DIR)
LDFLAGS      := -m elf_i386 -T link.ld
QEMUFLAGS    := -m 1g -monitor stdio -device VGA -drive format=raw,file=$(IMAGE),if=floppy

ENTRY        := kmain


.PHONY: all build run debug clean

all: $(IMAGE)

run: $(IMAGE)
	$(QEMU) $(QEMUFLAGS)

debug: $(IMAGE)
	$(QEMU) $(QEMUFLAGS) -s -S &
	gdb

clean:
	@rm -rf $(BUILD_DIR)

$(BOOTLOADER): $(BUILD_DIR) $(BOOT_SRC)
	$(NASM) -f elf32 -dSIZE=$(KER_SIZE_KB) $(BOOT_SRC) -o $@

$(KERNEL): $(BUILD_DIR) $(SOURCES) $(HEADERS) $(BOOTLOADER)
	$(CC) $(CFLAGS) -c $(SOURCES) -o $@

$(KERNEL_BIN): $(BUILD_DIR) $(BOOTLOADER) $(KERNEL)
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) -e $(ENTRY) $(BOOTLOADER) $(KERNEL)
	objcopy -I elf32-i386 -O binary $(KERNEL_ELF) $@


$(IMAGE): $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$< of=$@ conv=notrunc

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
