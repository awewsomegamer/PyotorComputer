CPPFLAGS =

all:
	gcc $(shell find . -type f -name '*.c') -o ../assembler.out -I./include/ $(CPPFLAGS)

debug: CPPFLAGS += -DDEBUG -g
debug: all