all:
	~/Downloads/llvm-mos/bin/mos-common-clang -Os -o bin/kernal.bin kernal.c

run: all
	../emaultor.out -load 0xDCC0 kernal/bin/kernal.bin