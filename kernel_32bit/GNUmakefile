ARCH ?= x86_32

OBJ_DIR := obj-$(ARCH)
BIN_DIR := bin-$(ARCH)

# Files varibles
OBJ := $(OBJ_DIR)/boot.o
BIN := $(BIN_DIR)/kernel_32bit

all: $(BIN)

# Creating output file
$(BIN): $(OBJ)
	@mkdir -p $(BIN_DIR)
	ld -m elf_i386 -Ttext 0x100000 -o $@ $^

# Creating bin file
$(OBJ): src/boot.asm
	@mkdir -p $(OBJ_DIR)
	nasm -f elf32 src/boot.asm -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: distclean
distclean:
	rm -rf obj-* bin-*