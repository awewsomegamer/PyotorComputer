CPPFLAGS = 

all:
	gcc src/*.c -o sysdbg.out -Isrc/include -lcurses $(CPPFLAGS)

debug: CPPFLAGS += -DDEBUG -g
debug: all