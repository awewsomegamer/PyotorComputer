DBG =
SPEED ?=

all:
	make -C assembler
	make -C emulator $(DBG) SPD=$(SPEED)
	make -C kernal
	make -C tools/filing_system
	make -C tools/sysdbg

clean:
	rm -rf ./bin/*

run: all
	./emulator.out -load 0xDCC0 ./kernal/kernal_rom.bin -disk 0 ./test.bin

debug: DBG += debug
debug: all