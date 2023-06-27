all:
	ca65 -g --cpu 65C02 kernal.s -o bin/kernal.o
	ld65 -C linker.ld65 -Ln labels.txt bin/*.o -o ./kernal_rom.bin

	# ~/Downloads/llvm-mos/bin/mos-common-clang -Os -o bin/kernal.bin kernal.c
	# cp bin/kernal.bin kernal_rom.bin

	make -C test_prg