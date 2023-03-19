        ; Set video port line high
        lda #1
        sta $1
        
        ; Write something to video memory
        lda #$FF
        sta $201

        ; Set video port line low
        lda #0
        sta $1