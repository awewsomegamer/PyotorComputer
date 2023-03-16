CPPFLAGS =

all:
	gcc $(shell find . -type f -name '*.c') -o ../emulator.out -I./include/ $(CPPFLAGS)
	xa test.as -o test.bin


debug: CPPFLAGS += -DDEBUG
debug: all