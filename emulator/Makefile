CPPFLAGS =

all:
	gcc $(shell find . -type f -name '*.c') -o ../emulator.out -I./include/ $(CPPFLAGS)
	ca65 asm/test.as --verbose -mm near --cpu 6502 -o asm/test.o
	ld65 -C asm/linker.ld65 -o test.bin asm/test.o


debug: CPPFLAGS += -DDEBUG
debug: all