        lda #$16
        sta $FFFE
        lda #$7F
        sta $FFFF

        brk
        nop

        stp

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
