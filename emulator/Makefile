CPPFLAGS =

all:
	gcc $(shell find ./src/ -type f -name '*.c') -o ../emulator.out -I./src/include/ $(CPPFLAGS) -lSDL2 -lm

	ca65 asm/kernal.asm --verbose -mm near --cpu 65C02 -o bin/kernal.o
	ld65 -C asm/linker.ld65 -o bin/kernal.bin bin/kernal.o

debug: CPPFLAGS += -DDEBUG
debug: all