CPPFLAGS =

all:
	gcc $(shell find . -type f -name '*.c') -o ../emulator.out -I./include/ $(CPPFLAGS)

	ca65 asm/kernel.as --verbose -mm near --cpu 65C02 -o bin/kernel.o
	ld65 -C asm/linker.ld65 -o bin/kernel.bin bin/kernel.o

	ca65 asm/boot.as --verbose -mm near --cpu 65C02 -o bin/boot.o
	ld65 -C asm/linker.ld65 -o bin/boot.bin bin/boot.o


debug: CPPFLAGS += -DDEBUG
debug: all