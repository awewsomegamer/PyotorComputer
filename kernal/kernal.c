#include <stdint.h>

int cursor_index = 0;
char *s = "Hello World";

void putc(char c) {
	*((uint16_t *)48512) = cursor_index; // Address
	*((uint8_t *)48514) = 0x03; // Terminal Mode
	*((uint8_t *)48515) = c; // Data
	*((uint8_t *)48516) = 0xC0; // Status
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
	set_fg(0xFF);

	puts(s);

	for (;;);

	return 0;
}