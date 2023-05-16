_entry:			lda #$FF
			sta 48517

			lda #'A'
			ldx #0
			ldy #0
			jsr putchar

_quit:			bra _quit

; A - Char
; X - Lower byte of address
; Y - Higher byte of address
putchar_nobg:		pha 		; Save A
			lda #%01010000 	; R/W | B
			bra _putchar
putchar: 		pha 		; Save A
	 		lda #%01000000 	; R/W
_putchar: 		sta 48516 	; Store in status
			pla 		; Restore A
			sta 48515 	; Store data
			stx 48512 	; Store lower half of address
			sty 48513 	; Store higher half of address
			lda #$03  	; 40x12 Terminal mode
			sta 48514 	; Store mode
			lda 48516 	; Load status
			ora #$80 	; | D1
			sta 48516 	; Store status
			rts

_irq_handler:
_nmi_handler: 		rti

.segment "VECTORS"
			.addr _irq_handler
			.addr _entry
			.addr _nmi_handler