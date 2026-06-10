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
			  src/io/io.s
C_SRC		= src/kernel/kernel.c \
			  src/drivers/vga/vga.c \
			  src/drivers/keyboard/keyboard.c
LINKER		= src/kernel.ld

BUILD		= build
NASM_OBJ	= $(patsubst src/%.s,$(BUILD)/%.o,$(NASM_SRC))
C_OBJ   	= $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRC))
OBJS 		= ${NASM_OBJ} ${C_OBJ}

NAME		= octor-os-kfs-1.elf
IMAGE 		= octor-os-kfs-1.img

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
	$(CC) $(CFLAGS) -I src/drivers/keyboard -I src/drivers/vga -I src/kernel -I src/io -c -o $@ $<

# Link
$(NAME): $(OBJS) $(LINKER)
	$(LD) -m elf_i386 -T $(LINKER) -o $@ $(OBJS)


# ───────────────────────────────────────────────────────────────
test: ${NAME}
	qemu-system-i386 -kernel ${NAME}

# Image disque + GRUB
# img: $(NAME)
# 	dd if=/dev/zero of=$(IMAGE) bs=1M count=32
# 	parted $(IMAGE) mklabel msdos
# 	parted $(IMAGE) mkpart primary ext2 1MiB 31MiB
# 	parted $(IMAGE) set 1 boot on
# 	sudo losetup -Pf --show $(IMAGE)         # → /dev/loopX
# 	@echo "Suite : mkfs, grub-install, copier kernel.elf — voir README"

# # Lance avec KVM
# run: $(IMAGE)
# 	qemu-system-i386 \
# 		-drive file=$(IMAGE),format=raw \
# 		-enable-kvm \
# 		-m 256M \
# 		-serial stdio

# Debug sans KVM (si KVM indispo)
# run-debug: $(IMAGE)
# 	qemu-system-i386 \
# 		-drive file=$(IMAGE),format=raw \
# 		-m 256M \
# 		-serial stdio \
# 		-s -S


# ───────────────────────────────────────────────────────────────
# Clean, re

clean:
	rm -f $(OBJS) $(NAME)
	rm -rf $(BUILD)

fclean: clean
	rm -f $(IMAGE)

re: fclean all

.PHONY: all clean fclean re