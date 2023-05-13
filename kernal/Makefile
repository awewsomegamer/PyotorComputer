all:
	make -C lib

	cc65 kernal.c -t none --cpu 65C02 --standard cc65 -Cl -o bin/kernal.s
	ca65 bin/kernal.s --cpu 65C02 -o bin/kernal.o

	ld65 -C linker.ld65 -o bin/kernal.bin lib/interrupt.o lib/vectors.o lib/wait.o bin/kernal.o lib/sbc.lib

run: all
	../emaultor.out -load 0xDCC0 kernal/bin/kernal.bin