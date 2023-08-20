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

			.define FS_CUR_DIR		48578 ; For initial directory and for current working directory
			.define FS_DIR_N_FREE_ENT	FS_CUR_DIR + 992
			.define FS_DIR_N_DIR_ENT_LO	FS_CUR_DIR + 993 
			.define FS_DIR_N_DIR_ENT_HI	FS_CUR_DIR + 994
			.define FS_DIR_N_FREE_SECT_LO	FS_CUR_DIR + 995 ; Only for initial descriptors
			.define FS_DIR_N_FREE_SECT_HI	FS_CUR_DIR + 996 ; Only for initial descriptors
			.define FS_DIR_VER_HI		FS_CUR_DIR + 997 ; Only for initial descriptors
			.define FS_DIR_VER_LO		FS_CUR_DIR + 998 ; Only for initial descriptors 
			.define FS_DIR_IDENTIFIER	FS_CUR_DIR + 999 ; Only for initial descriptors
			.define FS_FILE_ENTRY_SECT_LO	13
			.define FS_FILE_ENTRY_SECT_HI	14
			.define FS_FILE_ENTRY_SECT_ATTR	15

			.define FS_CUR_FILE		FS_CUR_DIR + 1024 ; For initial file descriptor and other file descriptors
			.define FS_FILE_N_DESC_LO	FS_CUR_FILE + 1000
			.define FS_FILE_N_DESC_HI	FS_CUR_FILE + 1001
			.define FS_FILE_ATTRS		FS_CUR_FILE + 1002
			.define FS_FILE_SZ_LO		FS_CUR_FILE + 1003 ; Only for initial descriptors
			.define FS_FILE_SZ_HI		FS_CUR_FILE + 1004 ; Only for initial descriptors
			.define FS_FILE_UNUSED_LO	FS_CUR_FILE + 1005 ; Only for initial descriptors
			.define FS_FILE_UNUSED_HI	FS_CUR_FILE + 1006 ; Only for initial descriptors

			.define MAX_FILES_PER_DIR	62
			.define FILE_NOT_FOUND_IDX	132

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
			rol a					; ASL A also produces a pretty cool result
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
			stz TERMINAL_CHAR_X			; Zero terminal X coordinate
			stz TERMINAL_CHAR_Y			; Zero terminal Y coordinate
			lda #$1					; Set disk number to 1
			jsr fs_initialize			; Initialize virtual file system from disk 1
			stz TERMINAL_STATUS			; Zero status
.endproc
			; Auto commands handling goes here
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

			lda #.LOBYTE(TERMINAL_BUFFER)		;
			sta $5					;
			lda #.HIBYTE(TERMINAL_BUFFER)		;
			sta $6					;
			lda #.LOBYTE(FG_CMD)			;
			sta $7					;
			lda #.HIBYTE(FG_CMD)			;
			sta $8					;
			jsr cmp_str				;
			bcc @bg_cmp				;
			ldy TERMINAL_BUFFER + 3			;
			ldx TERMINAL_BUFFER + 4			;
			jsr ascii_to_byte			;
			sta VIDEO_REG_FG			;
			jmp @compare_end			;

@bg_cmp:		lda #.LOBYTE(TERMINAL_BUFFER)		;
			sta $5					;
			lda #.HIBYTE(TERMINAL_BUFFER)		;
			sta $6					;
			lda #.LOBYTE(BG_CMD)			;
			sta $7					;
			lda #.HIBYTE(BG_CMD)			;
			sta $8					;
			jsr cmp_str				;
			bcc @jump_cmp				;
			ldy TERMINAL_BUFFER + 3			;
			ldx TERMINAL_BUFFER + 4			;
			jsr ascii_to_byte			;
			sta VIDEO_REG_BG			;
			jmp @compare_end			;

@jump_cmp:		lda #.LOBYTE(TERMINAL_BUFFER)		;
			sta $5					;
			lda #.HIBYTE(TERMINAL_BUFFER)		;
			sta $6					;
			lda #.LOBYTE(JUMP_CMD)			;
			sta $7					;
			lda #.HIBYTE(JUMP_CMD)			;
			sta $8					;
			jsr cmp_str				;
			bcc @dir_cmp				;

			ldy TERMINAL_BUFFER + 5			;
			ldx TERMINAL_BUFFER + 6			;
			jsr ascii_to_byte			;
			sta $6					;

			ldy TERMINAL_BUFFER + 7			;
			ldx TERMINAL_BUFFER + 8			;
			jsr ascii_to_byte			;
			sta $5					;

			stz TERMINAL_CHAR_X			;
			stz TERMINAL_CHAR_Y			;
			
			lda #.HIBYTE(@compare_end)		;
			pha					;
			lda #.LOBYTE(@compare_end)		;
			pha					;
			jmp ($5)				;

@dir_cmp:		lda #.LOBYTE(TERMINAL_BUFFER)		;
			sta $5					;
			lda #.HIBYTE(TERMINAL_BUFFER)		;
			sta $6					;
			lda #.LOBYTE(DIR_CMD)			;
			sta $7					;
			lda #.HIBYTE(DIR_CMD)			;
			sta $8					;
			jsr cmp_str				;
			bcc @load_cmp				;

			lda #$2					; Get the second argument
			jsr terminal_buffer_get_arg		; Get it
			txa					; Check how many characters were read
			beq @compare_end			; If zero were read, jump over
			lda TERMINAL_BUFFER, y			; Otherwise load in the character
			clc					; Clear the carry flag
			sbc #'A'				; Subtract A, to get disk index from A, B, C
			inc					; Increment it into a disk index

			jsr fs_list_directory			;
			jmp @compare_end			;


@load_cmp:		lda #.LOBYTE(TERMINAL_BUFFER)		;
			sta $5					;
			lda #.HIBYTE(TERMINAL_BUFFER)		;
			sta $6					;
			lda #.LOBYTE(LOAD_CMD)			;
			sta $7					;
			lda #.HIBYTE(LOAD_CMD)			;
			sta $8					;
			jsr cmp_str				;
			bcc @compare_end			;

			lda #.LOBYTE($400)			;
			sta $5					;
			lda #.HIBYTE($400)			;
			sta $6					;
			lda #.LOBYTE(TERMINAL_BUFFER + 5)	;
			sta $7					;
			lda #.HIBYTE(TERMINAL_BUFFER + 5)	;
			sta $8					;
			jsr fs_load_file			;
			
			ldy TERMINAL_CHAR_Y			;
			beq @load_no_sub			;
			dey					;
@load_no_sub:		lda #'A'				;
			jsr putchar				;
			jmp @compare_end			;

@compare_end:		ldy #$00				; Zero Y
			lda #$00				; Zero A
@reset_terminal_buffer:	cpy #TERMINAL_BUFFER_LEN		; See if we are at the end of the buffer
			beq @terminal_buffer_reset		; If so jump out of the loop
			sta TERMINAL_BUFFER, y			; Otherwise, zero the current character + y in the buffer
			iny					; Increment Y
			bra @reset_terminal_buffer		; Loop
@terminal_buffer_reset:	ply					; Restore Y
			pla					; Restore A
			stz TERMINAL_STATUS			; Zero status, we have handled it
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
			stx VIDEO_ADDR_LO			; Store the X coordinate
			sty VIDEO_ADDR_HI			; Store the Y coordinate
			sta VIDEO_REG_DATA			; Store the color
			pha					; Save A
			lda #$0					; 320x200 8-bit color mode
			sta VIDEO_REG_MODE			; Set the mode
			lda #%11000000				; D1 | R/W
			sta VIDEO_REG_STATUS			; Invoke the video register
			pla					; Restore A
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
			cmp #$0A				; Have we received a new line character?
			beq @new_line				; If so, print a new line, do not bother with this character
			jsr putchar				; Put the character (carry flag is set)			
			cpy #12					; Printed a character on the 12th, scroll, line or further?
			bcc @no_scroll				; If not then skip over the following
			ldy #11					; Set Y back to the last displayable line
@no_scroll:		inx					; Increment X
			cpx #40					; Are we at the end of a line?
			bne @no_new_line			; If not, no new line required, skip following
@new_line:		ldx #$00				; Zero X
			iny					; Increment Y
@no_new_line:		inc $05 				; Increment lower half
			bne @inc_lwr_addr_ovr 			; If the lower half didn't roll over to 0, jump over higher half incrementation
			inc $06 				; Increment higher half
@inc_lwr_addr_ovr:	bra @putstr 				; Loop
@end:		 	pla 					; Restore A
			rts					; Return
.endproc

; ($5) - String A (Compared against B)
; ($7) - String B
; Carry flag set = strings match
.proc cmp_str
@loop:			lda ($7)				; Load in the current character of the expected string
			cmp ($5)				; Compare it against the character of the input string
			bne @fail				; If they are not equal, goto fail
			cmp #$0					; Otherwise check if we reached a zero terminator
			beq @success				; If so, success
			inc $5					; Otherwise increment the low byte of the input string
			bne @inc_5_over				; Check to see if it rolled over
			inc $6					; If it did, increment the high byte too
@inc_5_over:		inc $7					; Increment the low byte of the expected string
			bne @inc_7_over				; Check to see if it rolled over
			inc $8					; If it did, increment the high byte too
@inc_7_over:		bra @loop				; Loop
@success:		sec					; Success, set the carry flag
			bra @end				; Branch to the end
@fail:			clc					; Fail, clear the carry flag
@end:			rts					; Return
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
			php					; Push flags
			cli					; Enable interrupts
			pha					; Save A
			lda #$1					; Load A with 1
			sta DISK_REG_STATUS			; Put it in the disk status #%00000001
			pla 					; Restore A
@left_shift:		dec 					; Decrement A register
			beq @ls_over 				; A is zero, we are done
			asl DISK_REG_STATUS 			; Bit shift disk bound bit left by 1
			bra @left_shift 			; Loop
@ls_over:		lda DISK_REG_STATUS			; Save the value (we need it for later)
			asl a					; Shift left
			asl a					; Shift left
			asl a					; Shift left
			pha					; Push the value to stack
			lda #$80 				; Set D1
			ora DISK_REG_STATUS			; Or D1 together with bound bit
			sta DISK_REG_STATUS			; Store final status
@wait:			wai					; Wait for disk IRQ
			pla					; Pull the mask off the stack
			pha					; Push it back just in case
			and DISK_REG_STATUS			; And it with the status
			beq @wait				; Has the disk been marked as completed? If not, wait
			pla					; Pull the just in case mask off
			lda DISK_REG_CODE			; Load A with the return code
			plp 					; Restore flags
			rts					; Return
.endproc

; A   - Disk to read (values must be: 1,2, or 3), contents
;       are not preserved.
; MEM - Set bytes DISK_BUFF_ADDR_LO, DISK_BUFF_ADDR_HI, DISK_SECTOR_LO, 
;       DISK_SECTOR_HI, DISK_SECTOR_COUNT_LO, DISK_SECTOR_COUNT_HI to the
;       apropriate values.
.proc write_disk
			php					; Push flags
			cli					; Enable interrupts
			pha					; Save A
			lda #$1					; Load A with 1
			sta DISK_REG_STATUS			; Put it in the disk status #%00000001
			pla 					; Restore A
@left_shift:		dec 					; Decrement A register
			beq @ls_over 				; A is zero, we are done
			rol DISK_REG_STATUS 			; Bit shift disk bound bit left by 1
			bra @left_shift 			; Loop
@ls_over:		lda DISK_REG_STATUS			; Save the value (we need it for later)
			asl a					; Shift left
			asl a					; Shift left
			asl a					; Shift left
			pha					; Push the value to stack
			lda #$C0 				; Set D1 | R/W
			ora DISK_REG_STATUS			; Or D1 together with bound bit
			sta DISK_REG_STATUS			; Store final status
@wait:			wai					; Wait for disk IRQ
			pla					; Pull the mask off the stack
			pha					; Push it back just in case
			and DISK_REG_STATUS			; And it with the status
			beq @wait				; Has the disk been marked as completed? If not, wait
			pla					; Pull the just in case mask off
			lda DISK_REG_CODE			; Load A with the return code
			plp 					; Restore flags
			rts
.endproc

; $5 - Destination, incremented to ($5) + ($7)
; $7 - Source, incremented to ($7) + ($9)
; $9 - Byte count, decremented to 0
.proc memcpy
			pha					; Save A register
@loop:			phx					; Save X register
			ldx #$00				; Load X with 0
			lda $9					; Load A with the low byte of the counter
			bne @cmp_low_over			; If the low byte is not 0, then jump over
			inx					; Otherwise, increment X
@cmp_low_over:		lda $A					; Load A with the high byte of the counter
			bne @cmp_high_over			; If the high byte is not 0, then jump over
			txa					; Otherwise, transfer X to A to update flags
			bne @complete				; If X wasn't 0 (incremented in previous step) then we are done
@cmp_high_over:		plx					; Otherwise, pull X off the stack

			lda ($7)				; Load in the current source byte
			sta ($5)				; Store it in the current destination byte
			
			dec $9					; Decrement lower byte of counter
			lda $9					; Load it
			cmp #$FF				; Compare it to see if it rolled over
			bne @dec_low_over_9			; If not, jump over
			dec $A					; Otherwise, decrement high byte
@dec_low_over_9:	inc $7					; Increment low byte of source
			bne @inc_low_over_7			; If not, jump over
			inc $8					; Otherwise, increment high byte
@inc_low_over_7:	inc $5					; Increment lower byte of destination
			bne @inc_low_over_5			; If not, jump over
			inc $6					; Otherwise increment high byte
@inc_low_over_5:	bra @loop				; Loop
@complete:		plx
			pla
			rts
.endproc

; A - The argument's index (one based)
; X - Returned as the count of characters within the
;     argument
; Y - Returned as the offset into the buffer at which
;     the start of the argument is located at
; No input registers are preserved
.proc terminal_buffer_get_arg
			ldy #$0					; Load character index with zero
@loop:			ldx #$0					; At the beginning of each loop, load character count with zero
			cmp #$0					; Have we counted through all arguments?
			beq @end				; If so, end
			pha					; Otherwise, save the current argument count
@advance_buffer:	lda TERMINAL_BUFFER, y			; Load in the current character
			beq @divider_found			; If we reached a "space" then we completed an argument
			iny					; If not increment the character index
			inx					; Increment the character count
			bra @advance_buffer			; Loop
@divider_found:		pla					; Restore the current argument index
			dec					; Decrement the argument index
			bra @loop				; If we are not at 0, loop
@end:			rts					; Return
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
; A - Returned 0 for success
.proc fs_initialize
			pha					; Save the disk number
			lda #.LOBYTE(FS_CUR_DIR)		; Load the low byte of the initial directory's address in RAM
			sta DISK_BUFF_ADDR_LO			; Store it to the right byte
			lda #.HIBYTE(FS_CUR_DIR)		; Load the higher byte of the initial directory's address in RAM
			sta DISK_BUFF_ADDR_HI			; Store it to the right byte
			stz DISK_SECTOR_LO			; Zero the lower byte of the sector index
			stz DISK_SECTOR_HI			; Zero the higher byte of the sector index
			lda #$2					; Two sectors will be read
			sta DISK_SECTOR_COUNT_LO		; Set the right byte
			stz DISK_SECTOR_COUNT_HI		; Zero the higher byte
			pla					; Restore the disk number
			jmp read_disk				; Jump to the read disk sub-routine (piggy backing off of its RTS statement)
.endproc

; A - Disk the file system is on
; A - Returned 9 for success
.proc fs_new_dir
.endproc

; A - Disk index
.proc fs_list_directory
			sei					; Disable interrupts
			jsr fs_initialize			; Initialize the initial directory descriptor
			lda #.LOBYTE(FS_CUR_DIR)		; Lower byte of the first entry's address
			sta $5					; Store it
			lda #.HIBYTE(FS_CUR_DIR)		; High byte of the first entry's address
			sta $6					; Store it
			stz $0					; Zero file index
@print_dir_loop:	ldy #$0F				; Load offset with 15, to check attributes block
			lda ($5), y				; Load in the attribute byte
			and #%11000000				; Select the D and E flags
			cmp #%01000000				; Check if the file is not deleted and exists
			bne @next				; If not goto the next file
			ldy #$00				; If so, zero the offset to print the name
@print_name_loop:	cpy #13					; Have we printed all of the characters in the name field?
			beq @print_name_end			; If so, branch to the end
			lda ($5), y				; Otherwise, load in the current character
			iny					; Increment the index
			phy					; Save the index
			ldx TERMINAL_CHAR_X			; Load the current X coordinate
			ldy TERMINAL_CHAR_Y			; Load the current Y coordinate
			jsr putchar				; Print the character
			cpy #12					; Have we printed a scroll character
			bcc @no_scroll				; If not, then jump over
			ldy #11					; Otherwise, load Y with the last row
@no_scroll:		inx					; Increment the X coordinate
			stx TERMINAL_CHAR_X			; Store the X coordinate
			sty TERMINAL_CHAR_Y			; Store the Y coordinate
			ply					; Restore the character index
			bra @print_name_loop			; Loop
@print_name_end:	inc TERMINAL_CHAR_Y			; Increment the Y coordinate
			stz TERMINAL_CHAR_X			; Zero the X coordinate to go to the beginning of the next line
@next:			lda $0					; Load file index
			jsr fs_dir_next_entry			; Read in the next entry
			cmp #FILE_NOT_FOUND_IDX			; Compare the A register to the maximum number of files per directory
			beq @end				; If A > MAX_FILES_PER_DIR then finish
			sta $0					; Save file index
@loop:			bra @print_dir_loop			; Otherwise, loop
@end:			cli					; Enable interrupts
			rts					; Return
.endproc

; ($7) - Address to file name (zero terminated)
; A - Returned as the file index or 132 if the file was not found
; ($5) - The base address of the entry in the current directory
.proc fs_find_file
			lda #$1					; Disk number (make this dynamic)
			jsr fs_initialize			; Load initialization directory
			lda #.LOBYTE(FS_CUR_DIR)		; Load the low byte of the first entry's base
			sta $5					; Store it
			lda #.HIBYTE(FS_CUR_DIR)		; Load the high byte of the first entry's base
			sta $6					; Store it
			phy					; Save the Y register
			stz $0					; Zero file index
@compare_loop_start:	ldy #$00				; Zero the Y register, to read the first character
@compare_loop:		cpy #13					; Compare the Y register to 14
			beq @no_match				; If it is equal, we have reached the end of the string, no match
			lda ($5), y				; Load the current character of the target name
			cmp ($7), y				; Compare it to the current entry's name
			bne @no_match				; If they are not equal, no match
			iny					; Increment to the next character
			cpy #12					; Have we read all characters in the name?
			bne @compare_loop			; If not, go back to the compare loop
			lda $0					; Load file index
			bra @return				; Branch to the return statement
@no_match:		lda $0					; Load file index
			jsr fs_dir_next_entry			; Get the next entry
			cmp #FILE_NOT_FOUND_IDX			; Have we read all entries?
			beq @return				; If so, return
			sta $0					; Save file index
			bra @compare_loop_start			; Go back to the loop starter
@return:		ply 					; Restore Y register
			rts					; Return
.endproc

; A - Disk the file is on (same as the one used for fs_initialize)
; ($5) - Base address of file entry
; A - Returned 0 for success
.proc fs_load_file_init
			pha					; Save disk number
			phy					; Save Y register
			ldy #FS_FILE_ENTRY_SECT_LO		; Load in the offset for sector low
			lda ($5), y				; Get the low byte
			sta DISK_SECTOR_LO			; Store it in the disk register
			ldy #FS_FILE_ENTRY_SECT_HI		; Load in the offset for sector high
			lda ($5), y				; Get the high byte
			sta DISK_SECTOR_HI			; Store it in the disk register
			stz DISK_SECTOR_COUNT_HI		; Zero the sector count high byte
			lda #$2					; Load A with 2 for the lower byte
			sta DISK_SECTOR_COUNT_LO		; Set the lower byte of the sector count
			lda #.LOBYTE(FS_CUR_FILE)		; Load the low byte of where the disk should be read to
			sta DISK_BUFF_ADDR_LO			; Store it
			lda #.HIBYTE(FS_CUR_FILE)		; Load the high byte of where the disk should be read to
			sta DISK_BUFF_ADDR_HI			; Store it
			ply					; Restore Y register
			pla					; Restore disk number
			jmp read_disk				; Read the disk
.endproc

; A - Disk the file is on (same as the one used for fs_initialize)
; A - Returned 0 for no new descriptors
.proc fs_load_next_file_desc
			pha					; Save disk number
			lda FS_FILE_N_DESC_LO			; Get the low byte
			sta DISK_SECTOR_LO			; Store it in the disk register
			ora FS_FILE_N_DESC_HI			; Or high and low bytes together
			beq @no_new_sector			; If the result is 0, there is no new sector to be read
			lda FS_FILE_N_DESC_HI			; Get the low byte
			sta DISK_SECTOR_HI			; Store it in the disk register
			stz DISK_SECTOR_COUNT_HI		; Zero the sector count high byte
			lda #$2					; Load A with 2 for the lower byte
			sta DISK_SECTOR_COUNT_LO		; Set the lower byte of the sector count
			lda #.LOBYTE(FS_CUR_FILE)		; Load the low byte of where the disk should be read to
			sta DISK_BUFF_ADDR_LO			; Store it
			lda #.HIBYTE(FS_CUR_FILE)		; Load the high byte of where the disk should be read to
			sta DISK_BUFF_ADDR_HI			; Store it
			pla					; Restore disk number
			jmp read_disk				; Read the disk
@no_new_sector:		pla					; Pull A off the stack
			lda #$00				; Return value is set to 0
			rts					; Return
.endproc

; ($5) - Load address
; ($7) - Address to file name
; Current dir should be the directory of the file
; A - Returned as 0 for success
.proc fs_load_file
			sei					; Disable interrupts
			lda $5					; Get the low byte of the load address
			pha					; Push the low byte to the stack
			lda $6					; Get the high byte of the load address
			pha					; Push the high byte to the stack
			jsr fs_find_file			; Find the file
			cmp #FILE_NOT_FOUND_IDX			; Was the file found?
			bne @over				; If so, jump over the following
			pla					; Remove high byte from stack
			pla					; Remove low byte from stack
			rts					; Return
@over:			lda #$1					; Load the disk number (make this dynamic)
			jsr fs_load_file_init			; Load file initializer
			pla					; Restore high byte
			sta $6					; Move it to high byte of destination
			pla					; Restore low byte
			sta $5					; Move it to low byte of destination
@loop:			lda #.LOBYTE(FS_CUR_FILE)		; Get the low byte of the source address
			sta $7					; Move it to low byte of source
			lda #.HIBYTE(FS_CUR_FILE)		; Get the high byte of the source address
			sta $8					; Move it to high byte of source
			lda #.LOBYTE(1000)			; Get the low byte of the counter
			sta $9					; Store it in the low byte of the counter
			lda #.HIBYTE(1000)			; Get the high byte of the counter
			sta $A					; Store it in the high byte of the counter
			jsr memcpy				; Memcpy
			lda #$1					; Load in the disk number (make this dynamic)
			jsr fs_load_next_file_desc		; Load in the next descriptor of the file
			beq @return				; If there are no more descriptors, return
			bra @loop				; Otherwise, loop
@return:		cli					; Enable interrupts
			rts
.endproc

; ($7) - Address to file name (zero terminated)
; ($5) - Address of the start of the data
; A - Returned as 0 for success
.proc fs_save_file
.endproc

; A - Current file index
; ($5) - Base address of the current entry
; A - Returned 132 if there are no more entries to read.
;     Otherwise it is the current file index
.proc fs_dir_next_entry
			cmp #MAX_FILES_PER_DIR			; Have we read all files in the directory?
			bne @next_entry				; If not, go to the next entry
			lda #$01				; If so, set the disk number (TODO: Make this dynamic)
			jsr fs_next_dir				; Read in the next directory
			bne @new_dir				; If it is not 0, we have a new directory
			lda #FILE_NOT_FOUND_IDX			; Otherwise, just say no file was found
			rts					; Return
@new_dir:		lda #.LOBYTE(FS_CUR_DIR)		; We have a new directory, reset the low byte of the base address
			sta $5					; Store it
			lda #.HIBYTE(FS_CUR_DIR)		; Reset the high byte of the base address
			sta $6					; Store it
			lda #$00				; Reset the file index
			rts					; Return
@next_entry:		pha					; Save file index
			lda $5					; Load in the current address
			sta $1					; Save the current address
			clc					; Clear the carry flag for addition step
			adc #$10				; Increment the address by 16, to go to the next entry
			sta $5					; Store the incremented lower byte
			cmp $1					; Compare the incremented lower byte to its previous value
			bcs @end				; If the new value is lower than the old one, we rolled over
			inc $6					; Since we rolled over, increment the higher byte
@end:			pla					; Restore file index
			inc					; Increment file index
			rts					; Return
.endproc

; A - Disk the file system is on
; A - Returned 0 means there are no more directories
.proc fs_next_dir
			pha					; Save the disk number
			lda #.LOBYTE(FS_CUR_DIR)		; Load the low byte of the initial directory's address in RAM
			sta DISK_BUFF_ADDR_LO			; Store it to the right byte
			lda #.HIBYTE(FS_CUR_DIR)		; Load the higher byte of the initial directory's address in RAM
			sta DISK_BUFF_ADDR_HI			; Store it to the right byte
			lda FS_DIR_N_DIR_ENT_LO			; Load in the next link's low byte
			sta DISK_SECTOR_LO			; Store it
			ora FS_DIR_N_DIR_ENT_HI			; Or the lower byte with the high byte
			beq @no_next_dir			; If the result is 0, there is no next directory
			lda FS_DIR_N_DIR_ENT_HI			; Load in the next link's high byte
			sta DISK_SECTOR_HI			; Store it
			lda #$2					; Two sectors will be read
			sta DISK_SECTOR_COUNT_LO		; Set the right byte
			stz DISK_SECTOR_COUNT_HI		; Zero the higher byte
			pla					; Restore the disk number
			jmp read_disk				; Jump to the read disk sub-routine (piggy backing off of its RTS statement)
@no_next_dir:		pla					; Restore A
			lda #$00				; Load error value
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
			lda CURRENT_PROGRAM_BASE		; Load in lower byte of program address
			ora CURRENT_PROGRAM_BASE + 1		; Or it with higher byte of program address 
			bne @terminal_over			; If the result is not 0, a program is running, jump over
			jsr terminal_handler			; Handle the terminal
@terminal_over:		lda CURRENT_PROGRAM_IRQ			; Check if user defined IRQ (non-zero)
			lda CURRENT_PROGRAM_IRQ			; Load in lower byte of program IRQ handler address
			ora CURRENT_PROGRAM_IRQ + 1		; or it with the higher byte of the program IRQ handler address
			beq @over				; If it 0, no custom handler, jump over
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
			lda CURRENT_PROGRAM_NMI			; Load A with the lower byte of the NMI handler address
			ora CURRENT_PROGRAM_NMI + 1		; Or A with the higher byte of the NMI handler address
			beq @over				; If the result is 0, no handler, then jump over
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
			.byte $00, $00, $00, $00
			.asciiz "-=[]\"
			.byte $00 ; SEMI-COLON
			.asciiz "'`,./"
			.byte $00 ; Caps lock

HEX_CHARS:		.asciiz "0123456789ABCDEF"
FG_CMD:			.asciiz "FG"
BG_CMD:			.asciiz "BG"
JUMP_CMD:		.asciiz "JUMP"
DIR_CMD:		.asciiz "DIR"
LOAD_CMD:		.asciiz "LOAD"
CMD_NO_MATCH:		.asciiz "Command parameters insufficient"

.segment "VECTORS"
			.addr nmi_handler
			.addr entry
			.addr irq_handler