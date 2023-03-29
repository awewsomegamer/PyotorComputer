        .org 56512

KERNAL_ENTRY:
        lda #$5
        adc #$5
        jmp KERNAL_ENTRY

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
        