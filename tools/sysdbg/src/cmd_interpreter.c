#include "include/cmd_interpreter.h"
#include "include/global.h"
#include "include/disassemble.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int command_buffer_idx = 0;
char command_buffer[512];
char command_response[512];

void execute_cmd() {
	// Can't do anything without a command
	if (command_buffer_idx == 0)
		return;

	memset(command_response, 0, 512);
	
	uint64_t hash = 0xA5B1C2;
	
	int i = 0;
	for (; i < command_buffer_idx && command_buffer[i] != ' '; i++)
		hash += char_hash(command_buffer[i]);

	switch (hash) {
	case CMD_BREAK_HASH: {
		int start_idx = ++i;
		for (; i < command_buffer_idx && (command_buffer[i] != ' ' || command_buffer[i] != 0); i++);
		char *symbol = (char *)malloc(i - start_idx + 1);
		memset(symbol, 0, i - start_idx + 1);
		strncpy(symbol, command_buffer + start_idx, (i - start_idx));

		int r = toggle_breakpoint(symbol);

		if (r == -1) {
			sprintf(command_response, "Failed to toggle break point");
			break;
		}
		
		sprintf(command_response, (r == 0) ? "Removed breakpoint" : "Added breakpoint");

		break;
	}
	}

	memset(command_buffer, 0, 512);
	command_buffer_idx = 0;
}

void cmd_receive_char(char c) {
	// Handle backspace
	if (command_buffer > 0 && (c == '\b' || c == 7))
		command_buffer[--command_buffer_idx] = 0;

	if (!isalnum(c) && c != ' ')
		return;

	command_buffer[command_buffer_idx++] = c;
}