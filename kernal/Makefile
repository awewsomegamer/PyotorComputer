all:
	ca65 --cpu 65C02 kernal.s -o bin/kernal.o

	ld65 -C linker.ld65 bin/*.o -o ./kernal_rom.bin