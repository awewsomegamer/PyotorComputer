CPPFLAGS = 

all:
	gcc src/*.c -o filer.out -Isrc/include $(CPPFLAGS)

debug: CPPFLAGS += -DDEBUG -g
debug: all