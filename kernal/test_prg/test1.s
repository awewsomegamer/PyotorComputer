.word $ECEC ; Eexecutable Signature
; .addr _irq_handler ; IRQ Handler Address
.word $0000
.word $0000 ; NMI Handler Address

			ldx #$0
			ldy #$0
			lda #$1
_draw_bg:		jsr $DDB1
			lda $0
			txa
			adc $0
			tya
			sbc $0
			asl a
			adc $0
			sta $0
			inx
			bne @over
			iny
			beq @end
@over:			bra _draw_bg
@end:			ldx #$0
			ldy #$0

			lda #$00
	 		sta 48518
_start:			lda #.LOBYTE(COOL_TEXT)
			sta $5
			lda #.HIBYTE(COOL_TEXT)
			sta $6
			lda #8
			jsr $DDC7
			cpy #$50
			beq _quit
			bra _start
_quit:			bra _quit

; _irq_handler:		lda #.LOBYTE(COOL_TEXT)
; 			sta $5
; 			lda #.HIBYTE(COOL_TEXT)
; 			sta $6
; 			lda #%00001000
; 			jsr $DDA1
; 			rti

COOL_TEXT:		.asciiz " Happy Father's Day  Boldog Apaknapjat "

.segment "END"