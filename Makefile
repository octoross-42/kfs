# ───────────────────────────────────────────────────────────────
# Toolchains, flags, files

CC			= i686-elf-gcc
ASM			= nasm
LD			= i686-elf-ld

CFLAGS 		= -m32 -ffreestanding -fno-builtin -fno-exceptions \
			  -fno-stack-protector -nostdlib -nodefaultlibs \
			  -Wall -Wextra -Werror
ASMFLAGS	= -f elf32


NASM_SRC	= src/boot/boot.s \
			  src/drivers/io/io.s \
			  src/lib/string/strlen.s \
			  src/lib/string/strncmp.s \
			  src/gdt/lgdt.s \
			  src/asm/get_registers.s

C_SRC		= src/kernel/kernel.c \
			  src/gdt/gdt.c \
			  src/drivers/vga/vga.c \
			  src/drivers/keyboard/keyboard.c \
			  src/drivers/serial/serial_com1.c \
			  src/lib/kprint/kfprintf.c \
			  src/lib/kprint/printk.c \
			  src/lib/ansi/ansi.c \
			  src/shell/shell.c \
			  src/debug/print_gdt_address.c \
			  src/debug/print_multiboot_data.c \
			  src/debug/print_stack.c

INCLUDE_DIR	= src/kernel \
			  src/debug \
			  src/drivers/io \
			  src/drivers/vga \
			  src/drivers/keyboard \
			  src/drivers/serial \
			  src/lib/kprint \
			  src/gdt \
			  src/lib	\
			  src/lib/string \
			  src/lib/ansi \
			  src/lib/ctype \
			  src/shell

LINKER		= src/kernel.ld


INCLUDE		= $(addprefix -I , $(INCLUDE_DIR))

BUILD		= build
NASM_OBJ	= $(patsubst src/%.s,$(BUILD)/%.o,$(NASM_SRC))
C_OBJ   	= $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRC))
OBJS 		= ${NASM_OBJ} ${C_OBJ}

NAME		= octor-os-kfs.elf
IMAGE 		= octor-os-kfs.iso

# ───────────────────────────────────────────────────────────────
# Colors, format

GREY    	=	\033[030m
RED     	=	\033[031m
GREEN   	=	\033[032m
YELLOW  	=	\033[033m
BLUE    	=	\033[034m
MAGENTA 	=	\033[035m
CYAN		=	\033[036m
BOLD		=	\033[1m
RESET   	=	\033[0m


# ───────────────────────────────────────────────────────────────
# Compilation

all: $(NAME)

$(BUILD)/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

# Link
$(NAME): $(OBJS) $(LINKER)
	$(LD) -m elf_i386 -T $(LINKER) -o $@ $(OBJS)


# ───────────────────────────────────────────────────────────────
# Image

image-installs:
	sudo apt install -y grub-pc-bin grub-common xorriso

$(IMAGE): $(NAME)
	mkdir -p iso/boot/grub
	cp $(NAME) iso/boot/
	echo 'set timeout=0\nset default=0\nmenuentry "octor-os" { multiboot /boot/$(NAME)\nboot }' > iso/boot/grub/grub.cfg
	grub-mkrescue -o $(IMAGE) iso/

# ───────────────────────────────────────────────────────────────
# Run

# run with kernel elf, not img, dev
run-dev: ${NAME}
	qemu-system-i386 -kernel ${NAME} -serial stdio

kvm-perms:
	sudo chmod 666 /dev/kvm

# run with KVM and os img
run: $(IMAGE)
	qemu-system-i386 \
		-drive file=$(IMAGE),format=raw \
		-enable-kvm \
		-m 256M \
		-serial stdio
# 			-> redirige COM1 (vm) vers stdio (host)

run-debug: $(IMAGE)
	gdb qemu-system-i386 \
		-s -S \
		-drive file=$(IMAGE),format=raw \
		-enable-kvm \
		-m 256M \
		-serial stdio


run-shell: $(IMAGE)
	qemu-system-i386 \
		-cdrom $(IMAGE) \
		-monitor stdio

# ───────────────────────────────────────────────────────────────
# Clean, re

clean:
	rm -f $(OBJS) $(NAME)
	rm -rf $(BUILD)

fclean: clean
	rm -f $(IMAGE)

re: fclean all

.PHONY: all clean fclean re