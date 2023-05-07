        .org 56512

KERNAL_ENTRY:
        cli ; Allow for interrupts

        ; Set foreground to white
        lda #$FF
        sta 48517

        ldx #$0
        ldy #$0

        ; Write data to disk
        ; Buffer address
        lda #.LOBYTE(COOL_DATA)
        sta 48519
        lda #.HIBYTE(COOL_DATA)
        sta 48520

        ; Disk address
        lda #$0
        sta 48521
        sta 48522

        ; Buffer length
        lda #$FF
        sta 48523
        lda #$0
        sta 48524

        ; Status
        lda #%11000001
        sta 48525

        ; Read data from disk
        ; Buffer address
        lda #.LOBYTE(OTHER_COOL_DATA)
        sta 48519
        lda #.HIBYTE(OTHER_COOL_DATA)
        sta 48520

        ; Disk address
        lda #$0
        sta 48521
        sta 48522

        ; Buffer length
        lda #$FF
        sta 48523
        lda #$0
        sta 48524

        ; Status
        lda #%10000001
        sta 48525

        ; Draw string
        lda #.LOBYTE(OTHER_COOL_DATA)
        sta $6
        lda #.HIBYTE(OTHER_COOL_DATA)
        sta $7
        jsr DRAW_STR

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

; $6 - Absolute address of string
; X - Char start index low
; Y - Char start index high
DRAW_STR:
        pha
        php
        clc

DRAW_STR_LOOP:
        lda ($6)
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
        lda $6
        adc 1
        sta $6
        bcc DRAW_STR_OVER2
        lda $7
        ina
        sta $7
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

COOL_DATA:
        .asciiz "Hello this is data from the kernal that has been written to a file because the kernel can do stuff"

OTHER_COOL_DATA:


; Interrupt Vectors
        .res $FFFA-*
        .org $FFFA

        .addr NMI_HANDLER
        .addr KERNAL_ENTRY
        .addr IRQ_HANDLER
        
