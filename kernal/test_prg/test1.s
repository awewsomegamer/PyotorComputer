.word $ECEC ; Eexecutable Signature
.addr _irq_handler ; IRQ Handler Address
.word $0000 ; NMI Handler Address

			lda #$FF
	 		sta 48518
; _start:			lda #.LOBYTE(COOL_TEXT)
; 			sta $5
; 			lda #.HIBYTE(COOL_TEXT)
; 			sta $6
; 			lda #8
; 			jsr $DD45
; 			cpy #$50
; 			beq _quit
; 			bra _start
_quit:			bra _quit

_irq_handler:		lda #.LOBYTE(COOL_TEXT)
			sta $5
			lda #.HIBYTE(COOL_TEXT)
			sta $6
			lda #8
			jsr $DD46
			rti

COOL_TEXT:		.asciiz "  ! H A V E  A  G O O D  S U M M E R !  "

.segment "END"