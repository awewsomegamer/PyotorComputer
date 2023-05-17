			.org 56512

			DISK_BUFF_ADDR_LO := 48519
			DISK_BUFF_ADDR_HI := 48520
			DISK_SECTOR_LO := 48521
			DISK_SECTOR_HI := 48522
			DISK_SECTOR_COUNT_LO := 48523
			DISK_SECTOR_COUNT_HI := 48524
			DISK_REG_STATUS := 48525
			DISK_REG_CODE := 48526


_entry:			lda #$FF
			sta 48517

			; Sector start number is 0
			lda #$2
			sta DISK_BUFF_ADDR_HI ; High byte of buffer address
			lda #$1
			sta DISK_SECTOR_COUNT_LO ; Low byte of sectors to read
			jsr read_disk

			lda #$0 ; #.LOBYTE(COOL_STRING)
			sta $5
			lda #$2 ; #.HIBYTE(COOL_STRING)
			sta $6
			jsr putstr

			lda #$1
			sta DISK_SECTOR_LO ; Sector number is 1
			lda #$2
			sta DISK_BUFF_ADDR_HI ; High byte of buffer address
			lda #$1
			sta DISK_SECTOR_COUNT_LO ; Low byte of sectors to read
			lda #$1 ; Disk number
			jsr read_disk
			jmp $0200 ; Jump to loaded program
			
_quit:			bra _quit

.proc putchar
; A - Char
; X - Lower byte of address
; Y - Higher byte of address
putchar_nobg:		pha 		; Save A
			lda #%01010000 	; R/W | B
			bra _putchar
putchar: 		pha 		; Save A
	 		lda #%01000000 	; R/W
_putchar: 		sta 48516 	; Store in status
			pla 		; Restore A
			sta 48515 	; Store data
			stx 48512 	; Store lower half of address
			sty 48513 	; Store higher half of address
			lda #$03  	; 40x12 Terminal mode
			sta 48514 	; Store mode
			lda 48516 	; Load status
			ora #$80 	; | D1
			sta 48516 	; Store status
			rts
.endproc

.proc putstr
; ($5) - Address of string
; X - Lower byte of address
; Y - Higher byte of address
putstr:			pha 			; Save A
_putstr:		lda ($5) 		; Load character
			beq _end 		; If the character is a '\0' terminate
			jsr putchar 		; Put the character
			inx 			; Increment the lower index address
			bne _inc_y_over 	; If it doesn't roll over, jump over higher index address
			iny 			; Increment the higher index address
_inc_y_over:		lda $05 		; Load lower half of character address
			inc 			; Increment
			sta $05 		; Store lower half of character address
			bne _inc_lwr_addr_ovr 	; If the lower half didn't roll over to 0, jump over
					      	; higher half incrementation
			lda $06 		; Load higher half of character address
			inc 			; Increment
			sta $06 		; Store higher of character address
_inc_lwr_addr_ovr:	bra _putstr 		; Loop
_end:			pla 			; Restore A
			rts
.endproc


.proc read_disk
; A - Disk to read (values must be: 1,2, or 3), contents
;     are not preserved.
; ($5) - Sector number
; ($7) - Memory buffer base address
; ($9) - Sectors to read
read_disk:		pha
			lda #$1
			sta DISK_REG_STATUS
			pla 
_left_shift:		dec 			; Decrement A register
			beq _ls_over 		; A is zero, we are done
			rol DISK_REG_STATUS 	; Bit shift disk bound bit left by 1
			bra _left_shift 	; Loop
_ls_over:		lda #$80 		; Set D1
			ora DISK_REG_STATUS	; Or D1 together with bound bit
			sta 48525		; Store final status
			; TODO - Add code to wait for the
			;	 disk operation to finish.

			; TODO - Add code to error check.
			; 	 If there was an error: print
			; 	 the code field in hex onto
			;	 the screen.

			rts
.endproc

_irq_handler:
_nmi_handler: 		rti

.segment "RODATA"
COOL_STRING:		.asciiz "Hello World, this is a longer string to test how good the putstr function is at its job. This string is very long, what a long string, so incredibly long there is just absolutely nothing to compare it to."

.segment "VECTORS"
			.addr _irq_handler
			.addr _entry
			.addr _nmi_handler