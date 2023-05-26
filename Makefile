all: asm emu kern

DBG =
SPEED ?=

asm:
	make -C assembler

emu:
	make -C emulator $(DBG) SPD=$(SPEED)

kern:
	make -C kernal

clean:
	rm -rf ./bin/*

run: all
	./emulator.out -load 0xDCC0 ./kernal/kernal_rom.bin -disk 0 ./kernal/test_prg/test1_prg_disk.bin

debug: DBG += debug
debug: all