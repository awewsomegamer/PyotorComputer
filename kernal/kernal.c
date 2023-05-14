#include <stdint.h>

uint16_t cursor_index = 0;
char *a = "Happy Mother's Day ";
char *h = "Boldog Anyak Napjat ";

void draw_pixel(int x, int y, uint8_t color) {
	*((uint16_t *)48512) = (uint16_t)(y * 320 + x); // Address
	*((uint8_t *)48514) = 0x00; // Terminal Mode
	*((uint8_t *)48515) = color; // Data
	*((uint8_t *)48516) = 0xC0; // Status
}

void putc(char c, uint8_t no_bg) {
	*((uint16_t *)48512) = cursor_index; // Address
	*((uint8_t *)48514) = 0x03; // Terminal Mode
	*((uint8_t *)48515) = c; // Data
	*((uint8_t *)48516) = 0b11000000 | ((no_bg & 1) << 4); // Status
	cursor_index++;
}

void puts(char *s, uint8_t no_bg) {
	for (int i = 0; s[i] != 0; i++)
		putc(s[i], no_bg);
}

void set_fg(char color) { *((uint8_t *)48517) = color; }
void set_bg(char color) { *((uint8_t *)48518) = color; }

void NMI_HANDLER() { }
void IRQ_HANDLER() { }

int main() {
	for (int i = 0; i < 200; i++)
		for (int j = 0; j < 320; j++)
			draw_pixel(j, i, ((i * j) % 0xFF) << 5);

	set_fg(0xFF);
	putc(' ', 1);
	puts(a, 1);
	puts(h, 1);

	for (;;);

	return 0;
}