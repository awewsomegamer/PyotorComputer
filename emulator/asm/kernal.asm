        .org 56512

KERNAL_ENTRY:
        cli ; Allow for interrupts

        lda #$FF ; Set foreground to white, leave background blank
        sta 48517
        
LOOP:
        sty 48512 ; Low byte of address 
        tya
        stx 48513 ; High byte of address
        lda #$00
        sta 48514 ; Mode

        sty 48515 ; Data

        lda #%11000000 ; Write to video memory
        sta 48516

        tya
        adc #$AD
        tay
        
        bcc OVER
        inx
OVER:
        cpx #$FF
        beq LOOP_END

        jmp LOOP

LOOP_END:
        ldx #$0
        ldy #$0

        lda #.LOBYTE(COOL_STRING)
        sta $4
        lda #.HIBYTE(COOL_STRING)
        sta $5
        jsr DRAW_STR

; Load address of sprite table
        lda #$00
        sta 48512 ; Low byte
        lda #$20
        sta 48513 ; High byte
        lda #$2
        sta 48514 ; Mode: Set sprite table address
        lda #%11000000
        sta 48516 ; Write to video memory

; Store smiley face at first sprite spot
        lda #%01000010
        sta $2000
        lda #%01000010
        sta $2001
        lda #%00000000
        sta $2002
        lda #%10000001
        sta $2003
        lda #%01111110
        sta $2004

; Change color
        lda #%00001011
        sta 48517

; Draw sprite
        lda #$FF
        sta 48512 ; Low byte
        lda #$0
        sta 48513 ; High byte
        lda #$0
        sta 48515
        lda #$1
        sta 48514 ; Mode: Draw sprite
        lda #%11010000
        sta 48516 ; Write to video memory

        wai
        nop

        stp


; $48515 - Char to draw
; X - Char index low
; Y - Char index high
DRAW_CHAR:
        pha
        stx 48512 ; Low byte of address
        sty 48513 ; High byte of address
        lda #$3   ; Terminal mode
        sta 48514
        lda #%11010000 ; Write to video memory
        sta 48516
        pla

        rts

; $4 - Absolute address of string
; X - Char start index low
; Y - Char start index high
DRAW_STR:
        pha
        php
        clc

DRAW_STR_LOOP:
        lda ($4)
        beq DRAW_STR_END

        sta 48515
        jsr DRAW_CHAR

; Increment index registers
        txa
        adc 1
        tax
        bcc DRAW_STR_OVER1
        iny
DRAW_STR_OVER1:
; Increment char pointer
        lda $4
        adc 1
        sta $4
        bcc DRAW_STR_OVER2
        lda $5
        ina
        sta $5
DRAW_STR_OVER2:
        jmp DRAW_STR_LOOP

DRAW_STR_END:
        plp
        pla

        rts

IRQ_HANDLER:
        phy
        ldy $3
        lda ALPHABET, y
        ply

        sta 48515
        jsr DRAW_CHAR

; Increment to next char
        php
        clc
        txa
        adc #1
        tax
        bcc IRQ_HANDLER_OVER
        iny
IRQ_HANDLER_OVER:
        plp

        rti

ALPHABET:
	.asciiz "    ABCDEFGHIJKLMNOPQRSTUVWXYZ"

NMI_HANDLER:
        rti

INCREMENTER:
        .byte $00

COOL_STRING:
        .asciiz "Hello World! I am writing this to you from a string and I am testing to see if screen wrapping works. Does it?"

; Interrupt Vectors
        .res $FFFA-*
        .org $FFFA

        .addr NMI_HANDLER
        .addr KERNAL_ENTRY
        .addr IRQ_HANDLER
        