; .word $ECEC

_start: 		lda #.LOBYTE(COOL_TEXT)
			sta $5
			lda #.HIBYTE(COOL_TEXT)
			sta $6
			lda #16
			jsr $DD3A
_quit:			bra _quit


COOL_TEXT:		.asciiz "Hello World, this message is now coming from a program that has just been loaded."

.segment "END"