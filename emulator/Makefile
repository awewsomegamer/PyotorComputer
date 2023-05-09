CPPFLAGS =

KERNAL_CFILES := $(shell find ./kernal/ -type f -name '*.c')
KERNAL_OFILES := $(KERNAL_CFILES:.c=.o)

all:
	gcc $(shell find ./src/ -type f -name '*.c') -o ../emulator.out -I./src/include/ $(CPPFLAGS) -lSDL2 -lm

	ca65 kernal/kernal.asm --cpu 65C02 -o bin/kernal_a.o

	cc65 kernal/kernal.c --cpu 65C02 -O --standard c99 -o bin/kernal.s
	ca65 bin/kernal.s --cpu 65C02 -o bin/kernal.o

	ld65 -C kernal/linker.ld65 -o bin/kernal.bin bin/kernal_a.o bin/kernal.o

debug: CPPFLAGS += -DDEBUG
debug: all