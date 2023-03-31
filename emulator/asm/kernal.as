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

        stp

DRAW_CHAR:
        stx 48512 ; Low byte of address
        sty 48513 ; High byte of address
        lda #$3
        sta 48514
        lda #%11010000 ; Write to video memory
        sta 48516
        
        rts

IRQ_HANDLER:
        rti        

NMI_HANDLER:
        rti

INCREMENTER:
        .byte $01


; Interrupt Vectors
        .res $FFFA-*
        .org $FFFA

        .addr NMI_HANDLER
        .addr KERNAL_ENTRY
        .addr IRQ_HANDLER
        