all:
	cp /usr/share/cc65/lib/supervision.lib sbc.lib
	ca65 --cpu 65c02 crt0.s
	ar65 a sbc.lib crt0.o
	
	ca65 --cpu 65c02 interrupt.s
	ca65 --cpu 65c02 vectors.s
	ca65 --cpu 65c02 wait.s
