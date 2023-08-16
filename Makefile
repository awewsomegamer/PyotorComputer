DBG =
SPEED ?=

all:
	make -C assembler $(DBG)
	make -C emulator $(DBG) SPD=$(SPEED)
	make -C kernal $(DBG)
	make -C tools/filing_system $(DBG)
	make -C tools/sysdbg $(DBG)

clean:
	rm -rf ./bin/*

run: all
	./emulator.out -load 0xDCC0 ./kernal/kernal_rom.bin -disk 0 ./test.bin

debug: DBG += debug
debug: all