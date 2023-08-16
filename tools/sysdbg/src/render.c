#include "include/render.h"
#include "include/global.h"
#include "include/shared_memory.h"
#include <curses.h>
#include <string.h>
#include <ctype.h>

void draw_window(char *label, int start_x, int end_x, int start_y, int end_y) {
	move(start_y, start_x);

	for (int i = start_x; i < end_x; i++)
		addch('-');
	addch('+');

	for (int i = start_y + 1; i < end_y; i++) {
		move(i, end_x);
		addch('|');
	}

	if (end_y < max_y) {
		move(end_y, start_x);

		for (int i = start_x; i < end_x; i++)
			addch('-');
		addch('+');
	}

	move(start_y, ((end_x + start_x) / 2) - (strlen(label) / 2));
	printw("%s", label);
}

int draw_disassembly(uint8_t *flags) {
	draw_window(DISASM_LABEL, LABEL_COLUMN, CODE_COLUMN_END, DISASM_START_ROW, DISASM_END_ROW);
	
	int cur_inst_len = 0;
	uint16_t last_pc = pc;

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
		free(str);
		
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

		if (i == DISASM_START_ROW + 1)
			cur_inst_len = pc - last_pc;
	}

	return cur_inst_len;
}

void draw_memdump_instruction_area(int x, int y, int cur_inst_len, uint16_t cur_pc) {
	int bound_low = -24;
	int bound_high = -16;

	for (int i = 0; i < 3; i++) {
		move(y++, x);
		for (int j = cur_pc - bound_high; j < cur_pc - bound_low; j++)
			printw("%02X ", *(memory + j));

		if (REGISTERS_END_COLUMN - REGISTERS_COLUMN > 50) {
			addch(' ');

			for (int j = cur_pc - bound_high; j < cur_pc - bound_low; j++)
				printw("%c ",  ((*(memory + j) > 0x20) ? *(memory + j) : 0));
		}

		bound_high += 8;
		bound_low += 8;
	}
	
	move(y++, x);
	for (int i = cur_pc; i < cur_pc + 8; i++) {
		if (i - cur_pc < cur_inst_len)
			HIGHLIGHT_ON

		printw("%02X ", *(memory + i));
		
		if (i - cur_pc < cur_inst_len)
			HIGHLIGHT_OFF
	}
	if (REGISTERS_END_COLUMN - REGISTERS_COLUMN > 50) {
		addch(' ');

		for (int i = cur_pc; i < cur_pc + 8; i++) {
			if (i - cur_pc < cur_inst_len)
				HIGHLIGHT_ON

			printw("%c ",  ((*(memory + i) > 0x20) ? *(memory + i) : 0));

			if (i - cur_pc < cur_inst_len)
				HIGHLIGHT_OFF
		}
	}

	bound_low = 8;
	bound_high = 16;

	for (int i = 0; i < 3; i++) {
		move(y++, x);
		for (int j = cur_pc + bound_low; j < cur_pc + bound_high; j++)
			printw("%02X ", *(memory + j));

		if (REGISTERS_END_COLUMN - REGISTERS_COLUMN > 50) {
			addch(' ');

			for (int j = cur_pc + bound_low; j < cur_pc + bound_high; j++)
				printw("%c ",  ((*(memory + j) > 0x20) ? *(memory + j) : 0));
		}

		bound_high += 8;
		bound_low += 8;
	}
}

void draw_registers(uint16_t cur_pc, int cur_inst_len) {
	draw_window(REGISTERS_LABEL, REGISTERS_COLUMN, REGISTERS_END_COLUMN, REGISTERS_START_ROW, REGISTERS_END_ROW);

	int y = REGISTERS_START_ROW + 1;
	int x = REGISTERS_COLUMN;

	move(y++, x);
	printw("A: %02X (%d)", *(memory + REGISTER_A_OFF), *(memory + REGISTER_A_OFF));
	move(y++, x);
	printw("X: %02X (%d)", *(memory + REGISTER_X_OFF), *(memory + REGISTER_X_OFF));
	move(y++, x);
	printw("Y: %02X (%d)", *(memory + REGISTER_Y_OFF), *(memory + REGISTER_Y_OFF));
	move(y++, x);
	printw("S: %02X (%d)", *(memory + REGISTER_S_OFF), *(memory + REGISTER_S_OFF));
	move(y++, x);
	printw("PC: %04X (%d)", cur_pc, cur_pc);

	move(y, x);
	printw("Flags: %02X (%d)", *(memory + REGISTER_P_OFF), *(memory + REGISTER_P_OFF));

	move(y - 1, REGISTERS_END_COLUMN - 8);
	printw("  Flags ");

	move(y++, REGISTERS_END_COLUMN - 8);
	if ((*(memory + REGISTER_P_OFF) >> 7) & 1) { GREEN_ON } else { RED_ON }
	addch('N');
	if ((*(memory + REGISTER_P_OFF) >> 7) & 1) { GREEN_OFF } else { RED_OFF }
	if ((*(memory + REGISTER_P_OFF) >> 6) & 1) { GREEN_ON } else { RED_ON }
        addch('V');
	if ((*(memory + REGISTER_P_OFF) >> 6) & 1) { GREEN_OFF } else { RED_OFF }
	if ((*(memory + REGISTER_P_OFF) >> 5) & 1) { GREEN_ON } else { RED_ON }
        addch('-');
	if ((*(memory + REGISTER_P_OFF) >> 5) & 1) { GREEN_OFF } else { RED_OFF }
	if ((*(memory + REGISTER_P_OFF) >> 4) & 1) { GREEN_ON } else { RED_ON }
        addch('B');
	if ((*(memory + REGISTER_P_OFF) >> 4) & 1) { GREEN_OFF } else { RED_OFF }
	if ((*(memory + REGISTER_P_OFF) >> 3) & 1) { GREEN_ON } else { RED_ON }
        addch('D');
	if ((*(memory + REGISTER_P_OFF) >> 3) & 1) { GREEN_OFF } else { RED_OFF }
	if ((*(memory + REGISTER_P_OFF) >> 2) & 1) { GREEN_ON } else { RED_ON }
        addch('I');
	if ((*(memory + REGISTER_P_OFF) >> 2) & 1) { GREEN_OFF } else { RED_OFF }
	if ((*(memory + REGISTER_P_OFF) >> 1) & 1) { GREEN_ON } else { RED_ON }
        addch('Z');
	if ((*(memory + REGISTER_P_OFF) >> 1) & 1) { GREEN_OFF } else { RED_OFF }
	if ((*(memory + REGISTER_P_OFF) >> 0) & 1) { GREEN_ON } else { RED_ON }
        addch('C');
	if ((*(memory + REGISTER_P_OFF) >> 0) & 1) { GREEN_OFF } else { RED_OFF }
	
	draw_window("INSTRUCTION IN MEMORY", REGISTERS_COLUMN, REGISTERS_END_COLUMN, y, REGISTERS_END_ROW);

	y++;

	draw_memdump_instruction_area(x, y, cur_inst_len, cur_pc);
}