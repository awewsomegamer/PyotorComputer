all: asm emu kern

DBG ?=

asm:
	make -C assembler

emu:
	make -C emulator $(DBG)

kern:
	make -C kernal

clean:
	rm -rf ./bin/*

run: all
	./emulator.out -load 0xDCC0 ./kernal/kernal_rom.bin

debug: DBG += debug
debug: all