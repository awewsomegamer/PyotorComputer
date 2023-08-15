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
	printw("PC: %02X (%d)", *(memory + CUR_INST_OFF), *(memory + CUR_INST_OFF));
	move(y++, x);
	printw("P: %02X (%d)", *(memory + REGISTER_P_OFF), *(memory + REGISTER_P_OFF));
	move(y++, x);
	printw("S: %02X (%d)", *(memory + REGISTER_S_OFF), *(memory + REGISTER_S_OFF));
	
	draw_window("INSTRUCTION IN MEMORY", REGISTERS_COLUMN, REGISTERS_END_COLUMN, y, REGISTERS_END_ROW);

	y++;

	int bound_low = -24;
	int bound_high = -16;

	for (int i = 0; i < 3; i++) {
		move(y++, x);
		for (int j = cur_pc - bound_high; j < cur_pc - bound_low; j++)
			printw("%02X ", *(memory + j));

		addch(' ');

		for (int j = cur_pc - bound_high; j < cur_pc - bound_low; j++)
			printw("%c ",  (!isblank(*(memory + j)) ? *(memory + j) : ' '));

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

	addch(' ');

	for (int i = cur_pc; i < cur_pc + 8; i++) {
		if (i - cur_pc < cur_inst_len)
			HIGHLIGHT_ON

		printw("%c ",  (!isblank(*(memory + i)) ? *(memory + i) : ' '));

		if (i - cur_pc < cur_inst_len)
			HIGHLIGHT_OFF
	}

	bound_low = 8;
	bound_high = 16;

	for (int i = 0; i < 3; i++) {
		move(y++, x);
		for (int j = cur_pc + bound_low; j < cur_pc + bound_high; j++)
			printw("%02X ", *(memory + j));

		addch(' ');

		for (int j = cur_pc + bound_low; j < cur_pc + bound_high; j++)
			printw("%c ",  (!isblank(*(memory + j)) ? *(memory + j) : ' '));

		bound_high += 8;
		bound_low += 8;
	}
}