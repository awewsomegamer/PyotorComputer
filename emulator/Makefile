CPPFLAGS =

KERNAL_CFILES := $(shell find ./kernal/ -type f -name '*.c')
KERNAL_OFILES := $(KERNAL_CFILES:.c=.o)

all:
	gcc $(shell find ./src/ -type f -name '*.c') -o ../emulator.out -I./src/include/ $(CPPFLAGS) -lSDL2 -lm

debug: CPPFLAGS += -DDEBUG -g
debug: all