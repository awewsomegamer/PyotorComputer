        .org 56512

KERNAL_ENTRY:
        lda #$1
        sta $0001
LOOP:
        lda #$1
        sta $0200, y
        iny
        jmp LOOP

IRQ_HANDLER:
        rti        

NMI_HANDLER:
        rti

; Interrupt Vectors
        .res $FFFA-*
        .org $FFFA

        .addr NMI_HANDLER
        .addr KERNAL_ENTRY
        .addr IRQ_HANDLER
        