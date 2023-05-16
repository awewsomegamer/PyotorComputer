CFLAGS := -Oz -ffreestanding

all:
	~/Downloads/llvm-mos/bin/mos-common-clang $(CFLAGS) -o bin/kernal.bin kernal.c
	~/Downloads/llvm-mos/bin/mos-common-clang $(CFLAGS) -S -o bin/kernal.s kernal.c

run: all
	../emaultor.out -load 0xDCC0 kernal/bin/kernal.bin

disasm: all
	~/Downloads/llvm-mos/bin/llvm-objdump -d -g --print-imm-hex bin/kernal.bin.elf
	hexdump -C bin/kernal.bin