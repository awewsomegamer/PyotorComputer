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

	uint64_t hash = 0xA5B1C2;
	
	int i = 0;
	for (; i < command_buffer_idx && command_buffer[i] != ' '; i++)
		hash += char_hash(command_buffer[i]);

	switch (hash) {
	case CMD_BREAK_HASH: {
		int start_idx = ++i;
		for (; i < command_buffer_idx && command_buffer[i] != ' '; i++);
		char *symbol = (char *)malloc(i - start_idx);
		strncpy(symbol, command_buffer + start_idx, (i - start_idx));

		if (toggle_breakpoint(symbol) == -1)
			// Error

		break;
	}
	}

	memset(command_buffer, 0, 512);
	command_buffer_idx = 0;
}

void cmd_receive_char(char c) {
	command_buffer[command_buffer_idx++] = c;
}