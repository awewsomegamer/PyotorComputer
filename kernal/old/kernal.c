#include <stdint.h>
#include <stddef.h>

uint16_t cursor_index = 0;

static uint16_t VECTOR_CONSTANTS[3] __attribute__((section(".vectors"))) __attribute__ ((__used__)) = {0xABAB, 0xBABA, 0xFAFA};

const char *guh = "GUH";
const uint8_t logo_bmp[] = {
	0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,							
	0b11111111, 0b11110111, 0b11100011, 0b11001001, 0b10011100,
	0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,
	0b11111111, 0b11111110, 0b11111100, 0b11111001, 0b11110011,
	0b00111110, 0b00000111, 0b11111111,0b11111111, 0b11111111,
	0b01111111, 0b00111111, 0b10011111, 0b11001111, 0b11100111, 
	
};

void draw_pixel(int x, int y, uint8_t color) {
	*((uint16_t *)48512) = (uint16_t)(y * 320 + x); // Address
	*((uint8_t *)48514) = 0x00; // Pixel Mode
	*((uint8_t *)48515) = color; // Data
	*((uint8_t *)48516) = 0xC0; // Status
}

void draw_sprite(int x, int y, uint8_t sprite_num, uint8_t no_bg) {
	*((uint16_t *)48512) = (uint16_t)(y * 40 + x); // Address
	*((uint8_t *)48514) = 0x01; // Sprite Mode
	*((uint8_t *)48515) = sprite_num; // Data
	*((uint8_t *)48516) = 0b11000000 | ((no_bg & 1) << 4); // Status
}

void putc(char c, uint8_t no_bg) {
	*((uint16_t *)48512) = cursor_index; // Address
	*((uint8_t *)48514) = 0x03; // Terminal Mode
	*((uint8_t *)48515) = c; // Data
	*((uint8_t *)48516) = 0b11000000 | ((no_bg & 1) << 4); // Status
	cursor_index++;
}

void puts(const char *s, uint8_t no_bg) {
	for (int i = 0; s[i] != 0; i++)
		putc(s[i], no_bg);
}

void set_fg(char color) { *((uint8_t *)48517) = color; }
void set_bg(char color) { *((uint8_t *)48518) = color; }

// The address of a variable cannot be passed as an
// integer value. Thus this does not work.
void set_sprite_table(uint16_t address) {
	*((uint16_t *)48512) = address; // Address
	*((uint8_t *)48514) = 0x02; // Set Sprite Table Mode
	*((uint8_t *)48516) = 0xC0; // Status
}

void NMI_HANDLER() { }
void IRQ_HANDLER() { }

int main() {
	set_fg(0xFF);

	// for (int i = 0; i < 200; i++)
	// 	for (int j = 0; j < 320; j++)
	// 		draw_pixel(j, i, i + j);

	// *((uint8_t *)0x200) = 0xFF;

	set_sprite_table((uintptr_t)&logo_bmp);

	int x = 0;
	int y = 0;
	for (int i = 0; i < 6; i++) {
		if (x == 3) {
			x = 0;
			y = y + 1;
		}

		draw_sprite(x, y, i, 1);
		x = x + 1;
	}

	puts(guh, 1);

	for (;;);

	return 0;
}

/*
A LOGO:

11111111 11111111 11111111
11111111 11110111 11111111
11111111 11100011 11111111
11111111 11001001 11111111
11111111 10011100 11111111

11111111 00111110 01111111
11111110 00000111 00111111
11111100 11111111 10011111
11111001 11111111 11001111
11110011 11111111 11100111

*/