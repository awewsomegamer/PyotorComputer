#include "include/cmd_interpreter.h"
#include "include/global.h"
#include "include/disassemble.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int command_buffer_idx = 0;
char command_buffer[512];

void execute_cmd() {
	// Can't do anything without a command
	if (command_buffer_idx == 0)
		return;

	size_t hash = 0x54A6734DF;
	
	int i = 0;
	for (; i < command_buffer_idx; i++) {
		hash += command_buffer[i];

		if (command_buffer[i] == ' ')
			break;
	}

	switch (hash) {
	case CMD_BREAK_HASH: {
		int start_idx = i;
		for (; i < 512 && command_buffer[i] != ' '; i++);
		char *symbol = (char *)malloc(i - start_idx);
		strncpy(symbol, command_buffer + start_idx + 1, (i - start_idx));

		toggle_breakpoint(symbol);

		break;
	}
	}
}

void cmd_receive_char(char c) {
	command_buffer[command_buffer_idx++] = c;
}