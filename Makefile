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

debug: DBG += debug
debug: all