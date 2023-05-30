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

			.define CURRENT_PROGRAM_BASE	48527
			.define CURRENT_PROGRAM_IRQ	48529
			.define CURRENT_PROGRAM_NMI	48531

			.define LOGO_WIDTH 		10
			.define LOGO_HEIGHT		10
			.define LOGO_START_X		15
			.define LOGO_START_Y		15
			.define LOGO_SPRITE_COUNT	LOGO_WIDTH * LOGO_HEIGHT

_entry:			lda #$00
			sta VIDEO_REG_FG
			lda #$00
			sta VIDEO_REG_BG
			
			ldx #$0
			ldy #$0
			lda #$1
_draw_bg:		jsr put_pixel
			lda $0
			txa
			adc $0
			tya
			sbc $0
			asl a
			adc $0
			sta $0
			inx
			bne @over
			iny
			beq @end
@over:			bra _draw_bg
@end:			ldx #$0
			ldy #$0
			
			lda #.LOBYTE(ASZ_LOGO_BMP)
			sta VIDEO_ADDR_LO
			lda #.HIBYTE(ASZ_LOGO_BMP)
			sta VIDEO_ADDR_HI
			lda #$2
			sta VIDEO_REG_MODE
			lda #$80
			sta VIDEO_REG_STATUS

			ldx #LOGO_START_X
			ldy #LOGO_START_Y
			lda #$0
			sta $0
_draw_logo:		stx VIDEO_ADDR_LO
			sty VIDEO_ADDR_HI
			lda #$1
			sta VIDEO_REG_MODE
			lda $0
			cmp #LOGO_SPRITE_COUNT
			beq @done
			sta VIDEO_REG_DATA
			inc
			sta $0
			lda #%11010000
			sta VIDEO_REG_STATUS
			inx
			cpx #(LOGO_WIDTH + LOGO_START_X)
			bne @over
			ldx #LOGO_START_X
			iny
			cmp #(LOGO_HEIGHT + LOGO_START_Y)
			beq @done
@over:			bra _draw_logo
@done:

			cli

			lda #$55
			sta $0200
			sta $0201
			sta $0202
			sta $0203
			sta $0204

			lda #$0
			sta DISK_BUFF_ADDR_LO
			sta $5
			lda #$2
			sta DISK_BUFF_ADDR_HI
			sta $6
			sta DISK_SECTOR_LO
			lda #$1
			sta DISK_SECTOR_COUNT_LO
			jsr write_disk

			lda #$1
			sta DISK_SECTOR_COUNT_LO
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
; A    - Foreground / Background mask (0: Draw both, 8: Draw only background, 16: Draw only foreground, 24: Draw none)
; X    - Lower byte of address
; Y    - Higher byte of address
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

; A   - Disk to read (values must be: 1,2, or 3), contents
;       are not preserved.
; MEM - Set bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI, DISK_SECTOR_LO, 
;       DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
;       apropriate values.
read_disk:		pha					; Save A
			lda #$1					; Load A with 1
			sta DISK_REG_STATUS			; Put it in the disk status #%00000001
			pla 					; Restore A
@left_shift:		dec 					; Decrement A register
			beq @ls_over 				; A is zero, we are done
			rol DISK_REG_STATUS 			; Bit shift disk bound bit left by 1
			bra @left_shift 			; Loop
@ls_over:		lda #$80 				; Set D1
			ora DISK_REG_STATUS			; Or D1 together with bound bit
			sta DISK_REG_STATUS			; Store final status
			; TODO - Add code to wait for the
			;	 disk operation to finish.

			; TODO - Add code to error check.
			; 	 If there was an error: print
			; 	 the code field in hex onto
			;	 the screen.

			rts

; A   - Disk to read (values must be: 1,2, or 3), contents
;       are not preserved.
; MEM - Set bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI, DISK_SECTOR_LO, 
;       DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
;       apropriate values.
write_disk:		pha					; Save A
			lda #$1					; Load A with 1
			sta DISK_REG_STATUS			; Put it in the disk status #%00000001
			pla 					; Restore A
@left_shift:		dec 					; Decrement A register
			beq @ls_over 				; A is zero, we are done
			rol DISK_REG_STATUS 			; Bit shift disk bound bit left by 1
			bra @left_shift 			; Loop
@ls_over:		lda #$C0 				; Set D1 | R/W
			ora DISK_REG_STATUS			; Or D1 together with bound bit
			sta DISK_REG_STATUS			; Store final status
			; TODO - Add code to wait for the
			;	 disk operation to finish.

			; TODO - Add code to error check.
			; 	 If there was an error: print
			; 	 the code field in hex onto
			;	 the screen.

			rts

; A   - Disk to read (values must be: 1,2, or 3), contents
;       are not preserved.
; MEM - Bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI should be set in
;       ($5).
;       Set bytes DISK_SECTOR_LO, DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
;       apropriate values.
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
			sta CURRENT_PROGRAM_BASE		; Set the low byte of the current program address
			cmp #$EC				; Ensure it meets the magic number
			bne _dont_run				; If it doesn't, error and return to caller
			iny					; Increment to the next byte of program
			lda ($5), y				; Load it
			sta CURRENT_PROGRAM_BASE + 1		; Set the high byte of the current progrma address
			cmp #$EC				; Ensure it meets the magic number
			bne _dont_run				; If it doesn't, error and return to caller
			iny					; IRQ Handler address low byte
			lda ($5), y				; Load it
			sta CURRENT_PROGRAM_IRQ			; Store it
			iny					; IRQ Handler address high byte
			lda ($5), y				; Load it
			sta CURRENT_PROGRAM_IRQ + 1		; Store it
			iny					; NMI Handler address low byte
			lda ($5), y				; Load it
			sta CURRENT_PROGRAM_NMI			; Store it
			iny					; NMI Handler address high byte
			lda ($5), y				; Load it
			sta CURRENT_PROGRAM_NMI + 1		; Store it
			ply					; Restore Y
			php					; Save status flags
			lda $5					; Load DISK_BUFF_ADDR_LO
			clc					; Clear carry flag
			adc #6					; Add with carry
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

_irq_handler:		; Kernal IRQ handler code here
			lda CURRENT_PROGRAM_IRQ			; Check if user defined IRQ (non-zero)
			php					; Load the current status into stack
			pla					; Load status into A
			sta $0					; Store in tmp value
			lda CURRENT_PROGRAM_IRQ + 1		; Check if user defined IRQ (non-zero)
			php					; Load the current status into stack
			pla					; Load status into A
			ora $0					; OR the two statuses together
			sta $0					; Store A back to 0 for BBS
			bbs1 $0, @over				; Jump over if the zero flag is set
			jmp (CURRENT_PROGRAM_IRQ)		; Jump to user handler
@over:			rti

_nmi_handler:		; Kernal NMI handler code here
			lda CURRENT_PROGRAM_NMI			; Check if user defined NMI (non-zero)
			php					; Load the current status into stack
			pla					; Load status into A
			sta $0					; Store in tmp value
			lda CURRENT_PROGRAM_NMI + 1		; Check if user defined NMI (non-zero)
			php					; Load the current status into stack
			pla					; Load status into A
			ora $0					; OR the two statuses together
			sta $0					; Store A back to 0 for BBS
			bbs1 $0, @over				; Jump over if the zero flag is set
			jmp (CURRENT_PROGRAM_NMI)		; Jump to user handler
@over:			rti
			

.segment "RODATA"
ASZ_LOGO_BMP: 		.byte 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 192, 128, 255, 255, 255, 0, 0, 255, 255, 255, 15, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 248, 0, 0, 0, 0, 0, 0, 48, 120, 252, 252, 3, 1, 1, 0, 0, 255, 255, 255, 255, 127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 240, 224, 192, 128, 128, 1, 3, 7, 15, 15, 254, 255, 255, 255, 255, 0, 0, 128, 192, 192, 63, 31, 15, 7, 3, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 248, 240, 0, 0, 0, 0, 1, 31, 63, 127, 255, 255, 255, 255, 255, 255, 255, 224, 240, 248, 252, 254, 3, 1, 0, 0, 0, 255, 255, 255, 127, 63, 255, 255, 255, 255, 255, 255, 255, 224, 224, 255, 255, 255, 0, 0, 254, 224, 192, 0, 0, 0, 1, 1, 0, 0, 31, 255, 255, 0, 3, 255, 255, 255, 255, 255, 255, 255, 255, 128, 0, 255, 255, 255, 0, 0, 255, 255, 255, 7, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 248, 240, 224, 0, 0, 0, 0, 1, 63, 63, 127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 240, 248, 248, 252, 254, 1, 0, 0, 0, 0, 255, 255, 127, 63, 31, 255, 255, 255, 255, 255, 192, 192, 255, 255, 255, 3, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 128, 255, 255, 255, 15, 15, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255

COULDNT_RUN_PROGRAM:	.asciiz "Could not run the program, does not start with the signature $ECEC"

.segment "VECTORS"
			.addr _nmi_handler
			.addr _entry
			.addr _irq_handler