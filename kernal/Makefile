all:
	#ca65 kernal.asm --cpu 65C02 -o bin/kernal_a.o

	cc65 old/kernal.c --cpu 65C02 -Oris -o bin/kernal.s
	ca65 bin/kernal.s --cpu 65C02 -o bin/kernal.o

	ld65 -C linker.ld65 -o bin/kernal.bin bin/kernal.o bin/kernal_a.o supervision.lib

run: all
	../emaultor.out -load 0xDCC0 kernal/bin/kernal.bin