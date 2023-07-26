all: asm emu kern fs

DBG =
SPEED ?=

asm:
	make -C assembler

emu:
	make -C emulator $(DBG) SPD=$(SPEED)

kern:
	make -C kernal

fs:
	make -C tools/filing_system

clean:
	rm -rf ./bin/*

run: all
	./emulator.out -load 0xDCC0 ./kernal/kernal_rom.bin -disk 0 ./test.bin

debug: DBG += debug
debug: all