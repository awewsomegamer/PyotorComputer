        lda #$16
        sta $FFFE
        lda #$7F
        sta $FFFF

        brk
        nop

        jmp #$0000

nop
nop
nop
nop
nop
nop
nop
nop
nop


IRQ_HANDLE:
        nop
        lda #$4
        rti
