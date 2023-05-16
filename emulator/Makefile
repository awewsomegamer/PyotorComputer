CPPFLAGS =

all:
	gcc $(shell find ./src/ -type f -name '*.c') -o ../emulator.out -I./src/include/ $(CPPFLAGS) -lSDL2 -lm

debug: CPPFLAGS += -DDEBUG -g
debug: all