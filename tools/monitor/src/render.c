#include "include/render.h"
#include "include/global.h"
#include <curses.h>
#include <string.h>

void draw_window(char *label, int start_x, int end_x, int start_y, int end_y) {
	move(start_y, start_x);

	for (int i = start_x; i < end_x; i++)
		addch('-');
	addch('+');

	for (int i = start_y + 1; i < end_y; i++) {
		move(i, end_x);
		addch('|');
	}

	move(start_y, ((end_x + start_x) / 2) - (strlen(label) / 2));
	printw("%s", label);
}

void draw_disassembly(uint8_t *flags) {
	draw_window(DISASM_LABEL, LABEL_COLUMN, CODE_COLUMN_END, DISASM_START_ROW, DISASM_END_ROW);

	for (int i = DISASM_START_ROW + 1; i < DISASM_END_ROW; i++) {
		char *str = print_instruction(memory, flags);
		char *printable_str = malloc(max_x + 1);
		memset(printable_str, 0, max_x + 1);

		int length = CODE_COLUMN_END - CODE_COLUMN - 5;
		int x = CODE_COLUMN;

		if (*flags & 1) {
			length = CODE_COLUMN - LABEL_COLUMN - 5;
			x = LABEL_COLUMN;
		}

		strncpy(printable_str, str, length);
		
		if (strlen(str) > length) {
			memset(printable_str + length, '.', 3);
			if (*flags & 1)
				printable_str[length + 3] = ':';
			printable_str[length + 4] = ' ';
		}

		if (i == DISASM_START_ROW + 1)
			HIGHLIGHT_ON

		move(i, x);
		printw("%s%c", printable_str, (*flags & 1 ? ':' : 0));

		if (i == DISASM_START_ROW + 1)
			HIGHLIGHT_OFF
		
		if (*flags & 1) {
			str = print_instruction(memory, flags);
			memset(printable_str, 0, max_x + 1);
			length = CODE_COLUMN_END - CODE_COLUMN - 5;

			strncpy(printable_str, str, length);

			if (strlen(str) > length) {
				memset(printable_str + length, '.', 3);
				if (*flags & 1)
					printable_str[length + 3] = ':';
			}
			
			if (i == DISASM_START_ROW + 1)
				HIGHLIGHT_ON

			move(i, CODE_COLUMN);
			printw("%s", printable_str);

			if (i == DISASM_START_ROW + 1)
				HIGHLIGHT_OFF

			free(str);
		}

		free(printable_str);
	}
}

void draw_registers() {
	draw_window(REGISTERS_LABEL, REGISTERS_COLUMN, REGISTERS_END_COLUMN, REGISTERS_START_ROW, REGISTERS_END_ROW);
}