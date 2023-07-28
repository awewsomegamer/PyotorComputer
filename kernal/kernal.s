			.org 56512

			.define VIDEO_ADDR_LO 		48512
			.define VIDEO_ADDR_HI 		48513
			.define VIDEO_REG_MODE 		48514
			.define VIDEO_REG_DATA 		48515
			.define VIDEO_REG_STATUS 	48516
			.define VIDEO_REG_FG 		48517
			.define VIDEO_REG_BG 		48518

			.define DISK_BUFF_ADDR_LO 	48519
			.define DISK_BUFF_ADDR_HI 	48520
			.define DISK_SECTOR_LO 		48521
			.define DISK_SECTOR_HI 		48522
			.define DISK_SECTOR_COUNT_LO 	48523
			.define DISK_SECTOR_COUNT_HI 	48524
			.define DISK_REG_STATUS 	48525
			.define DISK_REG_CODE 		48526
			
			.define IRQ_ACK_BYTE		48527

			.define CURRENT_PROGRAM_BASE	48528
			.define CURRENT_PROGRAM_IRQ	48530
			.define CURRENT_PROGRAM_NMI	48532

			.define TERMINAL_BUFFER 	48534
			.define TERMINAL_BUFFER_LEN	40
			.define TERMINAL_INDEX		48574
			.define TERMINAL_CHAR_X		48575
			.define TERMINAL_CHAR_Y		48576
			.define TERMINAL_STATUS		48577

			.define FS_INITIAL_DIR		48578
			.define FS_INIT_N_FREE_SECT_LO	FS_INITIAL_DIR + 992
			.define FS_INIT_N_FREE_SECT_HI	FS_INITIAL_DIR + 993
			.define FS_INIT_N_FREE_ENT	FS_INITIAL_DIR + 994
			.define FS_INIT_N_FREE_DIR_LO	FS_INITIAL_DIR + 995
			.define FS_INIT_N_FREE_DIR_HI	FS_INITIAL_DIR + 996
			.define FS_INIT_N_FREE_VER_HI	FS_INITIAL_DIR + 997
			.define FS_INIT_N_FREE_VER_LO	FS_INITIAL_DIR + 998
			.define FS_INIT_IDENTIFIER	FS_INITIAL_DIR + 999

			.define FS_CUR_DIR		FS_INITIAL_DIR + 1024
			.define FS_CUR_FILE		FS_CUR_DIR + 1024
			.define MAX_FILES_PER_DIR	62

			.define LOGO_WIDTH 		10
			.define LOGO_HEIGHT		10
			.define LOGO_START_X		15
			.define LOGO_START_Y		15
			.define LOGO_SPRITE_COUNT	LOGO_WIDTH * LOGO_HEIGHT
			
			.define CHAR_BACKSPACE		$2A
			.define CHAR_ENTER		$28

.proc entry
			stz VIDEO_REG_FG			; Set foreground to black
			stz VIDEO_REG_BG			; Set background to black
			ldx #$0					;
			ldy #$0					;
			lda #$1					;
@draw_bg:		jsr put_pixel				;
			lda $0					;
			txa					;
			adc $0					;
			tya					;
			sbc $0					;
			asl a					;
			adc $0					;
			sta $0					;
			inx					;
			bne @bg_over				;
			iny					;
			beq @end				;
@bg_over:		bra @draw_bg				;
@end:			ldx #$0					;
			ldy #$0					;
			lda #.LOBYTE(ASZ_LOGO_BMP)		;
			sta VIDEO_ADDR_LO			;
			lda #.HIBYTE(ASZ_LOGO_BMP)		;
			sta VIDEO_ADDR_HI			;
			lda #$2					;
			sta VIDEO_REG_MODE			;
			lda #$C0				;
			sta VIDEO_REG_STATUS			;
			ldx #LOGO_START_X			; Load the starting x-coordinate
			ldy #LOGO_START_Y			; Load the starting y-coordinate
			lda #$0					; Load the starting sprite index
			sta $0					; Zero temporary kernal value 0 (current sprite index)
@draw_logo:		stx VIDEO_ADDR_LO			; Set address low
			sty VIDEO_ADDR_HI			; Set address high
			lda #$1					; Draw sprite (8x5) video mode
			sta VIDEO_REG_MODE			; Set video mode
			lda $0					; Load current sprite index
			cmp #LOGO_SPRITE_COUNT			; Compare to maximum number of sprites allowed
			beq @done				; The routine is done if the maximum sprite number is reached
			sta VIDEO_REG_DATA			; The routine is not done yet, draw this sprite
			inc					; Increment to the next sprite
			sta $0					; Store the incremented sprite index
			lda #%11010000				; Load A with D1 | R/W | B
			sta VIDEO_REG_STATUS			; Set status
			inx					; Increment current x-coordinate
			cpx #(LOGO_WIDTH + LOGO_START_X)	; Is this x-coordinate at the right most edge of the image?
			bne @logo_over				; If not, jump over
			ldx #LOGO_START_X			; If so, reset x-coordinate to starting coordinate
			iny					; Increment y-coordinate
			cmp #(LOGO_HEIGHT + LOGO_START_Y)	; Is this y-coordinate at the bottom most edge of the image?
			beq @done				; The image is fully drawn
@logo_over:		bra @draw_logo				; Loop back
@done:			ldx #$0					; Zero X register
			ldy #$0					; Zero Y register
			sty $1					; Zero temporary value 1 (counter)
			lda #$05				; Get current second mode
			sta VIDEO_REG_MODE			; Store the mode
			lda #$80				; D1
			sta VIDEO_REG_STATUS			; Store the status
			lda VIDEO_REG_DATA			; Load A with the current second
			sta $0					; Store it in the temporary value
@check_rtc:		lda #$05				; Get current second mode
			sta VIDEO_REG_MODE			; Store the mode
			lda #$80				; D1
			sta VIDEO_REG_STATUS			; Store the status
			lda VIDEO_REG_DATA			; Load A with the current second	
			cmp $0					; Is the current time different from last time?
			beq @check_rtc				; If not, jump back to the loop
			sta $0					; Store the new second
			inc $1					; Increment counter
			lda $1					; Load in the amount of unique seconds passed
			cmp #$3					; Is the counter 3?
			beq @check_rtc_end			; If so, we're done (3 full seconds have passed)
			bra @check_rtc				; Otherwise, loop
@check_rtc_end:		ldx #$0					; Zero X
			stx VIDEO_REG_FG			; Store it in the foreground
			jsr clrscr				; Clear the screen
			ldx #$FF				; Set X to white color
			stx VIDEO_REG_FG			; Set foreground to white
			cli					; Enable interrupts
			stz TERMINAL_CHAR_X
			stz TERMINAL_CHAR_Y
			lda #$1					; Set disk number to 1
			jsr initialize_vfs			; Initialize virtual file system from disk 1
			stz TERMINAL_STATUS
.endproc

.proc terminal
			lda #%00000100				; Draw background, foreground, and enable hardware scroll
			sta VIDEO_REG_STATUS			; Set status to above
			stz CURRENT_PROGRAM_BASE		; No user program is running, zero
			stz CURRENT_PROGRAM_BASE + 1		; No user program is running, zero
			stz CURRENT_PROGRAM_IRQ			; No user program is running, zero
			stz CURRENT_PROGRAM_IRQ + 1		; No user program is running, zero
			stz CURRENT_PROGRAM_NMI			; No user program is running, zero
			stz CURRENT_PROGRAM_NMI + 1		; No user program is running, zero
			lda TERMINAL_STATUS			; Get the current status
			beq terminal				; It is zero, nothing new
			cmp #$1					; Something new? An enter I see
			beq enter_routine			; Let's goto the enter routine
			cmp #$2					; Something new? A regular character I see
			bne backspace_routine			; It is not a regular character, continue to backspace
			jmp reg_char_routine			; Regular character, let's goto the regular character routine
			; Something new, not matched, thus a backspace
backspace_routine:	ldx TERMINAL_CHAR_X			; Load the current X coordinate
			ldy TERMINAL_CHAR_Y			; Load the current Y coordinate
			cpy #$0
			bne @backspace_over
			cpx #$0
			bne @backspace_over
			stx TERMINAL_STATUS
			bra terminal
@backspace_over:	dex					; Decrement the X coordinate
			cpx #$FF				; Has X wrapped around to 0xFF?
			bne @backspace_x_over			; If not, do not decrement Y register
			dey					; Decrement the Y coordinate
@backspace_x_over:	stx TERMINAL_CHAR_X			; Store the current X coordinate
			sty TERMINAL_CHAR_Y			; Store the current Y coordinate
			ldx TERMINAL_INDEX			; Get the current terminal character index 
			stz TERMINAL_BUFFER, x			; Zero the character we are at
			dex					; Move the character index back one
			stx TERMINAL_INDEX			; Store the index
			lda #$0					; Zero A
			jsr putchar				; Print character
			lda #$0					; Zero A
			sta TERMINAL_STATUS			; Zero status, we have handled it
			bra terminal				; Go back to the start

enter_routine:		inc TERMINAL_CHAR_Y			; Goto next line
@enter_no_new:		pha					; Save A
			phy					; Save Y
			ldy #$0					; Zero Y
			sty TERMINAL_INDEX			; Zero Index
			sty TERMINAL_CHAR_X			; Zero X coordinate

			lda #.LOBYTE(TERMINAL_BUFFER)
			sta $5
			lda #.HIBYTE(TERMINAL_BUFFER)
			sta $6
			lda #.LOBYTE(FG_CMD)
			sta $7
			lda #.HIBYTE(FG_CMD)
			sta $8
			jsr cmp_str
			bcc @bg_cmp
			ldy TERMINAL_BUFFER + 3
			ldx TERMINAL_BUFFER + 4
			jsr ascii_to_byte
			sta VIDEO_REG_FG
			jmp @compare_end

@bg_cmp:		lda #.LOBYTE(TERMINAL_BUFFER)
			sta $5
			lda #.HIBYTE(TERMINAL_BUFFER)
			sta $6
			lda #.LOBYTE(BG_CMD)
			sta $7
			lda #.HIBYTE(BG_CMD)
			sta $8
			jsr cmp_str
			bcc @jump_cmp
			ldy TERMINAL_BUFFER + 3
			ldx TERMINAL_BUFFER + 4
			jsr ascii_to_byte
			sta VIDEO_REG_BG
			jmp @compare_end

@jump_cmp:		lda #.LOBYTE(TERMINAL_BUFFER)
			sta $5
			lda #.HIBYTE(TERMINAL_BUFFER)
			sta $6
			lda #.LOBYTE(JUMP_CMD)
			sta $7
			lda #.HIBYTE(JUMP_CMD)
			sta $8
			jsr cmp_str
			bcc @dir_cmp

			ldy TERMINAL_BUFFER + 5
			ldx TERMINAL_BUFFER + 6
			jsr ascii_to_byte
			sta $6

			ldy TERMINAL_BUFFER + 7
			ldx TERMINAL_BUFFER + 8
			jsr ascii_to_byte
			sta $5

			stz TERMINAL_CHAR_X
			stz TERMINAL_CHAR_Y
			
			lda #.HIBYTE(@compare_end)
			pha
			lda #.LOBYTE(@compare_end)
			pha
			jmp ($5)

@dir_cmp:		lda #.LOBYTE(TERMINAL_BUFFER)
			sta $5
			lda #.HIBYTE(TERMINAL_BUFFER)
			sta $6
			lda #.LOBYTE(DIR_CMD)
			sta $7
			lda #.HIBYTE(DIR_CMD)
			sta $8
			jsr cmp_str
			bcc @compare_end

			jsr vfs_list_directory

@compare_end:		ply					; Restore Y
			pla					; Restore A
			lda #$0					; Zero A
			sta TERMINAL_STATUS			; Zero status, we have handled it
			jmp terminal				; Start again

reg_char_routine:	lda $3					; Load A with what is in the keyboard buffer
			ldx TERMINAL_CHAR_X			; Load current X coordinate
			tay					; Write current scancode as offset
			lda ALPHABET, y				; Load ASCII character from table based on offset
			ldy TERMINAL_INDEX			; Get current index
			cpy #40					; Have we used up all of our buffer?
			bcs @reg_char_over			; If so, jump back to the terminal **** Breaks backspacing? ****
			sta TERMINAL_BUFFER, y			; Store the current character in the terminal buffer
			iny					; Increment index
			sty TERMINAL_INDEX			; Store the incremented index
			ldy TERMINAL_CHAR_Y			; Get the current Y coordinate
			pha					; Save the character to be printed
			lda #4					; Enable scroll
			sta VIDEO_REG_STATUS			; Update status
			pla					; Restore printed character
			jsr putchar				; Draw the character
			cpy #12					; Are we at the 12th line?
			bcc @reg_char_nl_ovr			; If not, jump over the following code
			ldy #11					; If so, set the current Y coordinate to the 11th line
			sty TERMINAL_CHAR_Y			; Update the Y coordinate
@reg_char_nl_ovr:	inx					; Increment the X coordinate
			bne @inx_over				; Didn't roll over, jump
			iny					; Rolled over, increment Y coordinate
@inx_over:		stx TERMINAL_CHAR_X			; Store X coordinate
			sty TERMINAL_CHAR_Y			; Store Y coordinate
@reg_char_over:		lda #$0					; Zero A
			sta TERMINAL_STATUS			; Zero status, we have handled it
			jmp terminal				; Start again
.endproc

; A - Char
; X - X coordinate
; Y - Y coordinate
.proc putchar
	 		sta VIDEO_REG_DATA 			; Store data
			stx VIDEO_ADDR_LO 			; Store lower half of address
			sty VIDEO_ADDR_HI 			; Store higher half of address
			lda #$03  				; 40x12 Terminal mode
			sta VIDEO_REG_MODE 			; Store mode
			lda VIDEO_REG_STATUS 			; Load status
			ora #%11000000 				; | D1 | R/W
			sta VIDEO_REG_STATUS 			; Store status
			rts					; Return
.endproc

; A - Color
; X - Lower byte of address
; Y - Higher byte of address
.proc put_pixel
			stx VIDEO_ADDR_LO
			sty VIDEO_ADDR_HI
			sta VIDEO_REG_DATA
			pha
			lda #$0					; 320x200 8-bit color mode
			sta VIDEO_REG_MODE
			lda #%11000000				; D1 | R/W
			sta VIDEO_REG_STATUS
			pla
			rts					; Return
.endproc

; ($5) - Address of string
; A    - Extra configuration (B(ackground disable) | F(oreground disable) | S(croll))
; X    - X coordinate
; Y    - Y coordinate
.proc putstr
			pha 					; Save A
@putstr:		pla					; Restore A
			sta VIDEO_REG_STATUS			; Write A to the status (will set B or F flags depending on user selection)
			pha					; Save A
			lda ($5) 				; Load character
			beq @end 				; If the character is a '\0' terminate
			jsr putchar 				; Put the character
			inx					; Increment X coordinate
			cpx #40					; Is it at the end of the row?
			bne @no_roll_over			; If not, jump over the following
			ldx #$0					; Reset X coordinate to the start of the row
			iny					; Go to the next column
@no_roll_over:		lda $05 				; Load lower half of character address
			inc 					; Increment
			sta $05 				; Store lower half of character address
			bne @inc_lwr_addr_ovr 			; If the lower half didn't roll over to 0, jump over
					      			; higher half incrementation
			lda $06 				; Load higher half of character address
			inc 					; Increment
			sta $06 				; Store higher of character address
@inc_lwr_addr_ovr:	bra @putstr 				; Loop
@end:		 	pla 					; Restore A
			rts					; Return
.endproc

; ($5) - String A (Compared against B)
; ($7) - String B
; Carry flag set = strings match
.proc cmp_str
@loop:			lda ($7)
			cmp ($5)
			bne @fail
			cmp #$0
			beq @success
			inc $5
			bne @inc_5_over
			inc $6
@inc_5_over:		inc $7
			bne @inc_7_over
			inc $8
@inc_7_over:		bra @loop
@success:		sec
			bra @end
@fail:			clc
@end:			rts
.endproc

; X - Low nibble (ASCII)
; Y - High nibble (ASCII)
; A - Returned byte
; X - Returned as $00 if error occurred
; Carry flag is cleared
.proc ascii_to_byte
			phy					; Save high nibble
			stz $1					; Zero temporary value 1
			stx $0					; Put low nibble in temporary value 0
			ldy #$0					; Zero indexing register
@low_byte:		lda HEX_CHARS, y			; Load in the next sequential hex character
			cmp $0					; Compare to low nibble
			beq @low_shift				; We found the character, jump to low_shift
			iny					; We have not found the character, goto next hex character
			cpy #16					; We could not find the character
			beq @error				; We have searched all hex characters, no match, error
			bra @low_byte				; Loop
@low_shift:		sty $1					; Save where we found it into temporary 1	
			ply					; Restore high nubble
			sty $0					; Put high nibble in temporary 0
			ldy #$0					; Zero indexing register
@high_byte:		lda HEX_CHARS, y			; Load in the next sequential hex character
			cmp $0					; Compare to high nibble
			beq @high_shift				; We found the character, jump to high_shift
			iny					; We have not found the character, goto next hex character
			cpy #16					; Have we searched all characters?
			beq @error				; We have searched all hex characters, no match, error
			bra @high_byte				; Loop
@high_shift:		tya					; Transfer index into A
			clc					; Ensure carry flag is not shifted into A register
			rol a					; Shift to bit 1
			rol a					; Shift to bit 2
			rol a					; Shift to bit 3
			rol a					; Shift to bit 4 (Into high nibble)
			ora $1					; Or together the lower nibble
			bra @end				; Goto end
@error:			ldx #$00				; Indicate we have encountered an error
@end:			rts					; Return
.endproc

; Foreground field in video register is the color to clear with
.proc clrscr
			phx					; Save X
			phy					; Save Y
			pha					; Save A
			; Save the previous sprite table address
			lda #$02				; Sprite Table Mode number
			sta VIDEO_REG_MODE			; Set current mode to Sprite Table Mode
			lda #$80 				; D1
			sta VIDEO_REG_STATUS			; Set status
			lda VIDEO_ADDR_LO			; Get low byte of sprite table address
			pha					; Save low byte of sprite table address
			lda VIDEO_ADDR_HI			; Get high byte of sprite table address
			pha					; Save high byte of sprite table address
			lda #$0B				; Load character buffer clear mode into A
			sta VIDEO_REG_MODE			; Set the mode to clear the character buffer
			lda #$80				; D1
			sta VIDEO_REG_STATUS			; Set status
			ldx #$0					; Zero x-coordinate
			ldy #$0					; Zero y-coordinate
@loop:			stx VIDEO_ADDR_LO			; Set x-coordinate
			sty VIDEO_ADDR_HI			; Set  y-coordinate
			pha					; Save A
			lda #$0					; Load A with 0
			sta VIDEO_REG_DATA 			; Set current sprite to sprite 0
			lda #$01				; Draw sprite mode
			sta VIDEO_REG_MODE			; Set the mode to Sprite Drawing Mode
			lda #$C0				; D1 | R/W
			sta VIDEO_REG_STATUS			; Set status, draw sprite
			pla					; Restore A
			inx					; Increment current x-coordinate
			cpx #40					; Do we need to go to the next line?
			bne @over_x				; We do not need to go to the next line
			ldx #$0					; Zero the x-coordinate
			iny					; Increment the y-coordinate
			cpy #40					; Is the screen cleared?
			beq @end				; The screen is cleared
@over_x:		jmp @loop				; Loop
@end:			; Restore the previous sprite table address
			pla					; Get the high byte of the previous sprite table address
			sta VIDEO_ADDR_HI			; Store it in the high byte of the address field
			pla					; Get the low byte of the previous sprite table address
			sta VIDEO_ADDR_HI			; Store it in the low byte of the address field
			lda #$02				; Sprite Table Address Mode
			sta VIDEO_REG_MODE			; Store it in the mode field
			lda #$C0 				; D1 | R/W
			sta VIDEO_REG_STATUS			; Put the address in the status field
			pla					; Restore A
			ply					; Restore Y
			plx					; Restore X
			rts
.endproc

; A   - Disk to read (values must be: 1,2, or 3), contents
;       are not preserved.
; MEM - Set bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI, DISK_SECTOR_LO, 
;       DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
;       apropriate values.
.proc read_disk
			pha					; Save A
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
.endproc

; A   - Disk to read (values must be: 1,2, or 3), contents
;       are not preserved.
; MEM - Set bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI, DISK_SECTOR_LO, 
;       DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
;       apropriate values.
.proc write_disk
			pha					; Save A
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

			rts					; Return
.endproc

; A   - Disk to read (values must be: 1,2, or 3), contents
;       are not preserved.
; MEM - Bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI should be set in
;       ($5).
;       Set bytes DISK_SECTOR_LO, DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
;       apropriate values.
.proc run_program
			pha					; Save A
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
			bne @dont_run				; If it doesn't, error and return to caller
			iny					; Increment to the next byte of program
			lda ($5), y				; Load it
			sta CURRENT_PROGRAM_BASE + 1		; Set the high byte of the current progrma address
			cmp #$EC				; Ensure it meets the magic number
			bne @dont_run				; If it doesn't, error and return to caller
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
@dont_run:		ply					; Restore Y
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
.endproc

; A - Disk the file system is on
.proc initialize_vfs
			pha					; Save the disk number
			lda #.LOBYTE(FS_INITIAL_DIR)		; Load the low byte of the initial directory's address in RAM
			sta DISK_BUFF_ADDR_LO			; Store it to the right byte
			lda #.HIBYTE(FS_INITIAL_DIR)		; Load the higher byte of the initial directory's address in RAM
			sta DISK_BUFF_ADDR_HI			; Store it to the right byte
			stz DISK_SECTOR_LO			; Zero the lower byte of the sector index
			stz DISK_SECTOR_HI			; Zero the higher byte of the sector index
			lda #$2					; Two sectors will be read
			sta DISK_SECTOR_COUNT_LO		; Set the right byte
			stz DISK_SECTOR_COUNT_HI		; Zero the higher byte
			pla					; Restore the disk number
			jmp read_disk				; Jump to the read disk sub-routine (piggy backing off of its RTS statement)
.endproc

.proc vfs_list_directory
			lda #.LOBYTE(FS_INITIAL_DIR)		; Lower byte of the first entry's address
			sta $5					; Store it
			lda #.HIBYTE(FS_INITIAL_DIR)		; High byte of the first entry's address
			sta $6					; Store it
			sei					; Disable interrupts
			stz $0					; Zero temporary value 0, used as file index counter
@print_dir_loop:	ldy #$0F				; Load offset with 15, to check attributes block
			lda ($5), y				; Load in the attribute byte
			and #%11000000				; Select the D and E flags
			cmp #$40				; Check if the file is not deleted and exists
			bne @next				; If not goto the next file
			ldy #$00				; If so, zero the offset to print the name
@print_name_loop:	lda ($5), y				; Load the current character of the name
			cpy #13					; Check if the offset is 13
			beq @print_name_end			; If so, we printed all characters
			iny					; Increment the offset
			phy					; Save the offset
			ldx TERMINAL_CHAR_X			; Load the current X coordinate
			ldy TERMINAL_CHAR_Y			; Load the current Y coordinate
			jsr putchar				; Print the current character
			inx					; Increment the X coordinate
			stx TERMINAL_CHAR_X			; Store the X coordinate
			ply					; Restore the offset
			bra @print_name_loop			; Loop back to the name printing loop
@print_name_end:	ldy TERMINAL_CHAR_Y			; Load the current Y coordinate
			cpy #12					; Are we at the end of the screen?
			bcc @no_nl				; If not, no new line is required
			ldy #11					; Step Y back to be on screen
@no_nl:			iny					; Increment the Y coordinate
			sty TERMINAL_CHAR_Y			; Store the Y coordinate
			stz TERMINAL_CHAR_X			; Zero the X coordinate to go to the beginning of the next line
@next:			lda $5					; Load in the current address
			sta $1					; Save the current address
			clc					; Clear the carry flag for addition step
			adc #$10				; Increment the address by 16, to go to the next entry
			sta $5					; Store the incremented lower byte
			cmp $1					; Compare the incremented lower byte to its previous value
			bcs @high_inc_over			; If the new value is lower than the old one, we rolled over
			inc $6					; Since we rolled over, increment the higher byte
@high_inc_over:		inc $0					; Increment the file index counter
			lda $0					; Load the file index counter into the A register
			cmp #MAX_FILES_PER_DIR			; Compare the A register to the maximum number of files per directory
			bcs @end				; If A > MAX_FILES_PER_DIR then finish
@loop:			bra @print_dir_loop			; Otherwise, loop
@end:			cli					; Enable interrupts
			rts					; Return
.endproc

.proc terminal_handler
			lda $3					; Load the current scancode
			cmp #CHAR_BACKSPACE			; Is it a backspace?
			bne @backspace_over			; Well it isn't
			lda #$3					; A backspace I see
			sta TERMINAL_STATUS			; Store the status
			rts					; Return
@backspace_over:	cmp #CHAR_ENTER				; Is it an enter?
			bne @enter_over				; Well it isn't
			lda #$1					; An enter I see
			sta TERMINAL_STATUS			; Store the status
			rts					; Return
@enter_over:		lda #$2					; Just a regular character
			sta TERMINAL_STATUS			; Store the status
			rts					; Return
.endproc

.proc irq_handler
			pha					; Save A
			phx					; Save X
			phy					; Save Y
			lda $0					; Load temporary value 0
			pha					; Save it
			lda $1					; Load temporary value 1
			pha					; Save it
			lda CURRENT_PROGRAM_BASE
			php
			pla
			sta $0
			lda CURRENT_PROGRAM_BASE + 1
			php
			pla
			ora $0
			sta $0
			bbr1 $0, @terminal_over
			jsr terminal_handler			; Handle the terminal
@terminal_over:		lda CURRENT_PROGRAM_IRQ			; Check if user defined IRQ (non-zero)
			php					; Load the current status into stack
			pla					; Load status into A
			sta $0					; Store in tmp value
			lda CURRENT_PROGRAM_IRQ + 1		; Check if user defined IRQ (non-zero)
			php					; Load the current status into stack
			pla					; Load status into A
			ora $0					; OR the two statuses together
			sta $0					; Store A back to 0 for BBS
			bbs1 $0, @over				; Jump over if the zero flag is set
			lda #.HIBYTE(@over)			; High byte of return address
			pha					; Push high byte
			lda #.LOBYTE(@over)			; Low byte of return address
			pha					; Push it
			jmp (CURRENT_PROGRAM_IRQ)		; Jump to user handler (basically a JSR due to prior push statements)
@over:			lda #$FF				; All interrupts have been taken care off
			sta IRQ_ACK_BYTE			; Let the IO controller know
			pla					; Restore temporary value 1
			sta $1					; Store it
			pla					; Restore temporary value 0
			sta $0					; Store it
			ply					; Restore Y
			plx					; Restore X
			pla					; Restore A
			rti					; Return from interrupt
.endproc

.proc nmi_handler
			pha					; Save A
			phx					; Save X
			phy					; Save Y
			lda $0					; Load temporary value 0
			pha					; Save it
			lda $1					; Load temporary value 1
			pha					; Save it
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
			lda #.HIBYTE(@over)			; High byte of return address
			pha					; Push high byte
			lda #.LOBYTE(@over)			; Low byte of return address
			pha					; Push it
			jmp (CURRENT_PROGRAM_NMI)		; Jump to user handler (basically a JSR due to prior push statements)
@over:			pla					; Restore temporary value 1
			sta $1					; Store it
			pla					; Restore temporary value 0
			sta $0					; Store it
			ply					; Restore Y
			plx					; Restore X
			pla					; Restore A
			rti					; Return from interrupt
.endproc
		
.segment "RODATA"
ASZ_LOGO_BMP: 		.byte 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 192, 128, 255, 255, 255, 0, 0, 255, 255, 255, 15, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 248, 0, 0, 0, 0, 0, 0, 48, 120, 252, 252, 3, 1, 1, 0, 0, 255, 255, 255, 255, 127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 240, 224, 192, 128, 128, 1, 3, 7, 15, 15, 254, 255, 255, 255, 255, 0, 0, 128, 192, 192, 63, 31, 15, 7, 3, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 248, 240, 0, 0, 0, 0, 1, 31, 63, 127, 255, 255, 255, 255, 255, 255, 255, 224, 240, 248, 252, 254, 3, 1, 0, 0, 0, 255, 255, 255, 127, 63, 255, 255, 255, 255, 255, 255, 255, 224, 224, 255, 255, 255, 0, 0, 254, 224, 192, 0, 0, 0, 1, 1, 0, 0, 31, 255, 255, 0, 3, 255, 255, 255, 255, 255, 255, 255, 255, 128, 0, 255, 255, 255, 0, 0, 255, 255, 255, 7, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 248, 240, 224, 0, 0, 0, 0, 1, 63, 63, 127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 240, 248, 248, 252, 254, 1, 0, 0, 0, 0, 255, 255, 127, 63, 31, 255, 255, 255, 255, 255, 192, 192, 255, 255, 255, 3, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 128, 255, 255, 255, 15, 15, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
COULDNT_RUN_PROGRAM:	.asciiz "Could not run the program, does not start with the signature $ECEC"
ALPHABET:		.asciiz "    ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
			.byte $00, $00, $00, $00, $00, $00, $00
HEX_CHARS:		.asciiz "0123456789ABCDEF"
FG_CMD:			.asciiz "FG"
BG_CMD:			.asciiz "BG"
JUMP_CMD:		.asciiz "JUMP"
DIR_CMD:		.asciiz "DIR"
CMD_NO_MATCH:		.asciiz "Command parameters insufficient"

.segment "VECTORS"
			.addr nmi_handler
			.addr entry
			.addr irq_handler