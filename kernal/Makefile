CPPFLAGS = 

all:
	ca65 -g --cpu 65C02 kernal.s -o bin/kernal.o
	ld65 -C linker.ld65 -Ln labels.txt bin/*.o -o ./kernal_rom.bin

	make -C test_prg

debug: CPPFLAGS += -DDEBUG -g
debug: all