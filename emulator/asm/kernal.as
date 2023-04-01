        .org 56512

KERNAL_ENTRY:
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

        lda #'H'
        sta 48515
        lda #$0
        jsr DRAW_CHAR
        
        lda #'e'
        sta 48515
        inx
        jsr DRAW_CHAR

        lda #'l'
        sta 48515
        inx
        jsr DRAW_CHAR

        lda #'l'
        sta 48515
        inx
        jsr DRAW_CHAR

        lda #'o'
        sta 48515
        inx
        jsr DRAW_CHAR

        inx

        lda #'W'
        sta 48515
        inx
        jsr DRAW_CHAR

        lda #'o'
        sta 48515
        inx
        jsr DRAW_CHAR

        lda #'r'
        sta 48515
        inx
        jsr DRAW_CHAR

        lda #'l'
        sta 48515
        inx
        jsr DRAW_CHAR

        lda #'d'
        sta 48515
        inx
        jsr DRAW_CHAR

        inx

        lda #.LOBYTE(COOL_STRING)
        sta $4
        lda #.HIBYTE(COOL_STRING)
        sta $5
        jsr DRAW_STR

        ply
        plx

        stp


; $4815 - Char to draw
; X - Char index low
; Y - Char index high
DRAW_CHAR:
        pha
        stx 48512 ; Low byte of address
        sty 48513 ; High byte of address
        lda #$3
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
        rti        

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
        