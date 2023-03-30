        .org 56512

KERNAL_ENTRY:
LOOP:
        sty 48512 ; Low byte of address 
        tya
        stx 48513 ; Middle byte of address
        lda #$00
        sta 48514 ; High byte of address

        sty 48515 ; Data

        lda #%11000000 ; Write to video memory
        sta 48516

        tya
        adc #$1
        tay
        
        bcc OVER
        inx
OVER:

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
        