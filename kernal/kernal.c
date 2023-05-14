#include <stdint.h>

int cursor_index = 0;
char *s = "Happy Mother's Day";
char *h = "Boldog Anyak Napjat";

void draw_pixel(uint16_t address, uint8_t color) {
	*((uint16_t *)48512) = address; // Address
	*((uint8_t *)48514) = 0x00; // Terminal Mode
	*((uint8_t *)48515) = color; // Data
	*((uint8_t *)48516) = 0xC0; // Status
}

void putc(char c) {
	*((uint16_t *)48512) = cursor_index; // Address
	*((uint8_t *)48514) = 0x03; // Terminal Mode
	*((uint8_t *)48515) = c; // Data
	*((uint8_t *)48516) = 0b11010000; // Status
	cursor_index++;
}

void puts(char *s) {
	int i = 0;
	for (i = 0; s[i] != 0; i++)
		putc(s[i]);
}

void set_fg(char color) { *((uint8_t *)48517) = color; }
void set_bg(char color) { *((uint8_t *)48518) = color; }

void NMI_HANDLER() { }
void IRQ_HANDLER() { }


int main() {
	int x = 0;

	for (; x < 10; x++) {
		int i = 0;
		int j = 0;
		
		// for (; j < 200; j++)
		// 	for (; i < 320; i++)
		// 		draw_pixel(i, j, ((i + j) % 0xFF) << 5);
		
		// i = 0;

		// for (; i < 20; i++) {
		// 	set_fg(i << 5);
		// 	puts(s);
		// 	putc(' ');
		// 	puts(h);
		// 	putc(' ');
		// }

		i = 0;
		for (; i < 1000; i = i + 1) {
			draw_pixel(i, 0xFF);
			int p = 0;
			for (; p < 1000; p++);
		}
	}


	for (;;);

	return 0;
}