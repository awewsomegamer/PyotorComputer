all:
	ca65 --cpu 65C02 test1.s -o bin/test1.o
	ld65 -C linker.ld65 bin/test1.o -o bin/test1.bin

	cat ../../emulator/disk1.bin bin/test1.bin > test1_prg_disk.bin