all: asm emu


asm:
	make -C assembler

emu:
	make -C emulator

clean:
	rm -rf ./bin/*

