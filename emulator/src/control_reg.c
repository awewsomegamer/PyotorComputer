#include <control_reg.h>
#include <ram.h>
#include <stdio.h>
#include <video.h>
#include <audio.h>
#include <disk.h>
#include <cpu/cpu.h>

struct control_register {
        uint16_t address;        // Address 								 @ 48512 -> 48513
        uint8_t mode;            // Mode								 @ 48514
        uint8_t data;            // Data field								 @ 48515
        uint8_t status;          // D(ata)1 R(ead)/W(rite) D(ata)2 B F 0 0 0				 @ 48516
                                 // D1: Indicates new data from CPU -> Video card			 
                                 // R/W: 0: reading data, 1: writing data				 
                                 // D2: Indicates new data from Video card -> CPU			 
                                 // B: 0: Draw background, 1: Don't draw background (text mode, sprite mode)	
				 // F: 0: Draw foreground, 1: Don't draw foreground (text mode, sprite mode)	 
        uint8_t foreground;      // Foreground to use for text						 @ 48517
        uint8_t background;      // Background to use for text (carry set means background is not drawn) @ 48518
}__attribute__((packed));

struct disk_register {
	uint16_t buffer_address; // Address of the buffer to read to, or to write from			 @ 48519 -> 48520					
				 // in general purpose RAM						 		
	uint16_t sector;   	 // Sector on the disk							 @ 48521 -> 48522				
	uint16_t buffer_length;  // The length of the buffer (in 512 byte sectors)			 @ 48523 -> 48524
	uint8_t status; 	 // D1 R/W C3 C2 C1 B3 B2 B1						 @ 48525	
			 	 // D1: 1: New data from CPU to Disk					 			
			 	 // R/W: 0: Read, 1: Write						 		
			 	 // C3: 1: Disk 3 completed operation					 			
			 	 // C2: 1: Disk 2 completed operation					 			
			 	 // C1: 1: Disk 1 completed operation					 			
			 	 // B3: 1: Operation bound for disk 3					 			
			 	 // B2: 1: Operation bound for disk 2					 			
			 	 // B1: 1: Operation bound for disk 1					 			
	uint8_t code;		 // The exit code of an operation 					 @ 48526			
				 // ((disk) 0, (disk) 1, or (disk) 2 are success codes)
}__attribute__((packed));

/*
	Control Register Context in different Modes

	Bit 7 and Bit 5 of the status field are always cleared
	when Bit 7 is detected to be set.

	Fields standard across all modes:
		uint8_t mode ->       A numerical value which indicates the task to preform.
				      This value is set by the CPU.

		uint8_t status ->
			Bit 7 (D1):   Flag to notify the controller that there is new
			              data from the CPU to the Controller.
			Bit 6 (R/W):  Indicates the direction of the data, reading from
				      or writing to the controller. 0: Read, 1: Write
			Bit 5 (D2):   Flag to notify the CPU that there is new data from
				      the controller to the CPU.
			Bit 4 (B):    This bit only applies to the video controller, and
				      simply states whether to draw the background of a
				      video operation or not. 0: Draw background, 1:
				      Don't draw background.
			Bit 3:        This bit is like bit 4 but instead applies to the
				      foreground.
			Bit 2:        Unused
			Bit 1:        Unused
			Bit 0:        Unused

	Mode 0x00:                    Read / Write byte of Video Memory (320x200 8-bit color)
		uint16_t address   -> The address of the byte in VRAM (0x0000 -> 0xFA00)
		uint8_t data       -> 8-bit color to place at the address
		
	Mode 0x01:		      Draw a sprite at the given index (40x40, each sprite being 8x5)
		uint16_t address   -> The index of the sprite in VRAM (in which 8x5 block in VRAM to draw the sprite)
		uint8_t data	   -> The index of the sprite in the bitmap (which sprite to draw)
		uint8_t foreground -> The foreground color of the sprite (color to use on all 1's of the sprite bitmap)
		uint8_t background -> The background color of the sprite (color to use on all 0's of the sprite bitmap) *
		* Note: Bit 4 of the status byte applies to this mode


	Mode 0x02:		      Set the address of the bitmap containing the 256 
		uint16_t address   -> The base address of the bitmap in general RAM

	Mode 0x03:		      40x12 Terminal Mode
		uint16_t address   -> The 8x16 index in which to draw the ASCII character
		uint8_t data	   -> The ASCII code of the character
		uint8_t foreground -> The foreground color of the sprite (color to use on all 1's of the sprite bitmap)
		uint8_t background -> The background color of the sprite (color to use on all 0's of the sprite bitmap) *
		* Note: Bit 4 of the status byte applies to this mode


	Mode 0x04:		      40x12 Terminal Mode
		uint16_t address   -> The 16-bit frequency to play
		uint8_t data	   -> The duration of the sound *
		* Note: When bit 7 is 0: The duration is (1 / value) seconds.
			When bit 7 is 1: The duration is (1 * value) seconds.

	Disk Control Register

	All bits besides Bit 6 are cleared when Bit 7 is
	detected to be set.

	This makes it possible for ONLY ONE disk to be
	bound for operation. If more than one disk is
	bound for operation, then an error will be thrown.

	The error is in the form of an IRQ with the code
	field of the disk control register being a non-
	zero value.
*/


void tick_control_register() {
	struct control_register *reg = (struct control_register *)(general_memory + KERNAL_DAT_BASE); // Temporary "KERNAL_DAT_BASE"
	
	if ((reg->status >> 7) & 1) {
		reg->status &= ~(1 << 7); // Clear D1
		reg->status &= ~(1 << 5); // Clear D2
		
		switch (reg->mode) {
		case 0x00: // 320x200 8-bit Color
			// We are outside of the screen, do not write.
			if (reg->address >= VRAM_SIZE)
				break;

			if (((reg->status >> 6) & 1) == 0) { 
				// Read
				reg->data = video_mem_read(reg->address);
				reg->status |= (1 << 5); // Set D2
			} else {
				// Write
				video_mem_write(reg->address, reg->data);
			}

			break;
		
		case 0x01: { // Sprite Mode (40x40)
			// 8 bytes wide, 5 bytes tall
			video_draw_sprite(reg->address, reg->data, reg->foreground, reg->background, ((reg->status >> 4) & 1) | (((reg->status >> 3) & 1) << 1));

			break;
		}

		case 0x02: // Set / Get Sprite Table Address
			if (((reg->status >> 6) & 1) == 0) { 
				// Read
				reg->address = video_get_sprite_table_address();
			} else {
				// Write
				video_set_sprite_table_address(reg->address);
			}

			break;
		
		case 0x03: // 40x12 Terminal Mode
			// Address behaves like index into screen: y * 40 + x
			video_draw_character(reg->address, reg->data, reg->foreground, reg->background, ((reg->status >> 4) & 1) | (((reg->status >> 3) & 1) << 1));

			break;
		
		case 0x04: // Play 16-bit sound
			tick_speaker(reg->address, reg->data);

			break;
		}
	}

	struct disk_register *disk_reg = (struct disk_register*)(general_memory + KERNAL_DAT_BASE + sizeof(struct control_register));
	if ((disk_reg->status >> 7) & 1) {
		disk_reg->code = disk_operation_buffer(disk_reg->buffer_length, disk_reg->buffer_address,
						       disk_reg->status & 0b111, disk_reg->sector, (disk_reg->status >> 6) & 1);

		disk_reg->status &= 0b01000000; // Clear all bits except for R/W

		if (disk_reg->code >= 0 && disk_reg->code <= 2)
			disk_reg->status |= disk_reg->code << 3;

		pin_IRQ = 0; // Call interrupt to notify CPU disk operation is done (code and status fields are set)
	}
}