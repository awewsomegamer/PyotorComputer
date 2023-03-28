CPPFLAGS =

all:
	gcc $(shell find . -type f -name '*.c') -o ../emulator.out -I./include/ $(CPPFLAGS)

	ca65 asm/kernal.as --verbose -mm near --cpu 65C02 -o bin/kernal.o
	ld65 -C asm/linker.ld65 -o bin/kernal.bin bin/kernal.o

debug: CPPFLAGS += -DDEBUG
debug: all