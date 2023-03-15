CPPFLAGS =

all:
	gcc $(shell find . -type f -name '*.c') -o ../emulator.out -I./include/ $(CPPFLAGS)

debug: CPPFLAGS += -DDEBUG
debug: all