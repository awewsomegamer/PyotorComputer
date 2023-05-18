			.org 56512

			.define DISK_BUFF_ADDR_LO 	48519
			.define DISK_BUFF_ADDR_HI 	48520
			.define DISK_SECTOR_LO 		48521
			.define DISK_SECTOR_HI 		48522
			.define DISK_SECTOR_COUNT_LO 	48523
			.define DISK_SECTOR_COUNT_HI 	48524
			.define DISK_REG_STATUS 	48525
			.define DISK_REG_CODE 		48526

			.define VIDEO_ADDR_LO 		48512
			.define VIDEO_ADDR_HI 		48513
			.define VIDEO_REG_MODE 		48514
			.define VIDEO_REG_DATA 		48515
			.define VIDEO_REG_STATUS 	48516
			.define VIDEO_REG_FG 		48517
			.define VIDEO_REG_BG 		48518


_entry:			lda #$FF
			sta VIDEO_REG_FG
			lda #$FF
			sta VIDEO_REG_BG

			; Sector start number is 0
			lda #$2
			sta DISK_BUFF_ADDR_HI 			; High byte of buffer address
			lda #$1
			sta DISK_SECTOR_COUNT_LO 		; Low byte of sectors to read
			jsr read_disk
			
			; ldx #$0
			; ldy #$0
			; lda #$1
; _draw_bg:		jsr put_pixel
; 			inx
; 			bne @over
; 			iny
; 			beq @end
; 			beq @end
; @over:			bra _draw_bg
; @end:			ldx #$0
; 			ldy #$0

			lda #$0 				; #.LOBYTE(COOL_STRING)
			sta $5
			lda #$2 				; #.HIBYTE(COOL_STRING)
			sta $6
			lda #%00001000
			jsr putstr

			lda #$0
			sta $5
			lda #$2
			sta $6
			lda #$1
			sta DISK_SECTOR_LO
			jsr run_program


_quit:			bra _quit

; A - Char
; X - Lower byte of address
; Y - Higher byte of address
putchar: 		sta VIDEO_REG_DATA 			; Store data
			stx VIDEO_ADDR_LO 			; Store lower half of address
			sty VIDEO_ADDR_HI 			; Store higher half of address
			lda #$03  				; 40x12 Terminal mode
			sta VIDEO_REG_MODE 			; Store mode
			lda VIDEO_REG_STATUS 			; Load status
			ora #%11000000 				; | D1 | R/W
			sta VIDEO_REG_STATUS 			; Store status
			rts

; A - Color
; X - Lower byte of address
; Y - Higher byte of address
put_pixel:		stx VIDEO_ADDR_LO
			sty VIDEO_ADDR_HI
			sta VIDEO_REG_DATA
			pha
			lda #$0					; 320x200 8-bit color mode
			sta VIDEO_REG_MODE
			lda #%11000000				; D1 | R/W
			sta VIDEO_REG_STATUS
			pla
			rts

; ($5) - Address of string
; A - Foreground / Background mask (0: Draw both, 16: Draw only foreground, 32: Draw only background, 48: Draw none)
; X - Lower byte of address
; Y - Higher byte of address
putstr:			pha 					; Save A
_putstr:		pla					; Restore A
			sta VIDEO_REG_STATUS			; Write A to the status (will set B or F flags depending on user selection)
			pha					; Save A
			lda ($5) 				; Load character
			beq _end 				; If the character is a '\0' terminate
			jsr putchar 				; Put the character
			inx 					; Increment the lower index address
			bne _inc_y_over 			; If it doesn't roll over, jump over higher index address
			iny 					; Increment the higher index address
_inc_y_over:		lda $05 				; Load lower half of character address
			inc 					; Increment
			sta $05 				; Store lower half of character address
			bne _inc_lwr_addr_ovr 			; If the lower half didn't roll over to 0, jump over
					      			; higher half incrementation
			lda $06 				; Load higher half of character address
			inc 					; Increment
			sta $06 				; Store higher of character address
_inc_lwr_addr_ovr:	bra _putstr 				; Loop
_end:		 	pla 					; Restore A
			rts

; A - Disk to read (values must be: 1,2, or 3), contents
;     are not preserved.
; Set bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI, DISK_SECTOR_LO, 
; DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
; apropriate values.
read_disk:		pha					; Save A
			lda #$1					; Load A with 1
			sta DISK_REG_STATUS			; Put it in the disk status #%00000001
			pla 					; Restore A
_left_shift:		dec 					; Decrement A register
			beq _ls_over 				; A is zero, we are done
			rol DISK_REG_STATUS 			; Bit shift disk bound bit left by 1
			bra _left_shift 			; Loop
_ls_over:		lda #$80 				; Set D1
			ora DISK_REG_STATUS			; Or D1 together with bound bit
			sta 48525				; Store final status
			; TODO - Add code to wait for the
			;	 disk operation to finish.

			; TODO - Add code to error check.
			; 	 If there was an error: print
			; 	 the code field in hex onto
			;	 the screen.

			rts

; A - Disk to read (values must be: 1,2, or 3), contents
;     are not preserved.
; Bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI should be set in
; ($5)
; Set bytes DISK_SECTOR_LO, DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
; apropriate values.
run_program:		pha					; Save A
			lda $5					; Load DISK_BUFF_ADDR_LO
			sta DISK_BUFF_ADDR_LO			; Store it in its proper place
			lda $6					; Load DISK_BUFF_ADDR_HI
			sta DISK_BUFF_ADDR_HI			; Store it in its proper place
			pla					; Restore A
			jsr read_disk 				; Read in the sector of the disk
			phy					; Save Y
			ldy #$0					; 0 the Y register
			lda ($5), y				; Load in the first byte of program
			cmp #$EC				; Ensure it meets the magic number
			bne _dont_run				; If it doesn't, error and return to caller
			iny					; Increment to the next byte of program
			lda ($5), y				; Load it
			cmp #$EC				; Ensure it meets the magic number
			bne _dont_run				; If it doesn't, error and return to caller
			ply					; Restore Y
			php					; Save status flags
			lda $5					; Load DISK_BUFF_ADDR_LO
			clc					; Clear carry flag
			adc #2					; Add with carry
			sta $5					; Store it back to its place
			bcc @over				; If the operation didn't roll over, jump over this block
			lda $6					; Load DISK_BUFF_ADDR_HI
			inc					; Increment it once
			sta $6					; Store it back to its place
@over:			plp					; Restore status flags
			jmp ($5)				; Jump into program, the program can return to
								; caller by calling an RTS
_dont_run:		ply
			lda #.LOBYTE(COULDNT_RUN_PROGRAM)	; Load lower half of the address of the error string
			sta $5					; Store it
			lda #.HIBYTE(COULDNT_RUN_PROGRAM)	; Load higher half of the address of the error string
			sta $6					; Store it
			lda #%11100000				; Set background color to red (ERROR!!!!!)
			sta VIDEO_REG_BG			; Store it in the video register
			lda #$FF				; Set the foreground color to white
			sta VIDEO_REG_FG			; Store it in the video register
			lda #$0					; Draw both background and foreground
			jsr putstr				; Put it on the screen
			lda #$FF				; Error code $FF
			rts					; Return to caller (code $FF, error occurred)
								; * For some reason the above RTS returns to weird places *


_irq_handler:
_nmi_handler: 		rti

.segment "RODATA"
COULDNT_RUN_PROGRAM:	.asciiz "Could not run the program, does not start with the signature $ECEC"

.segment "VECTORS"
			.addr _irq_handler
			.addr _entry
			.addr _nmi_handler