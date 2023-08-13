#include "include/disassemble.h"
#include <stdio.h>

int pc = 0;

char *print_instruction(uint8_t *buffer) {
	char *instruction_str = malloc(3);

	uint8_t opcode = *(buffer + pc++);

	sprintf(instruction_str, "%s", instruction_name_lookup[opcode]);

	switch (instruction_addr_mode_lookup[opcode]) {
	case AM_ABS: {
		// Implement label resolution here
		uint16_t arg = *(buffer + pc++) | (*(buffer + pc++) << 8);
		instruction_str = realloc(instruction_str, 8);
		sprintf(instruction_str + 3, " %04X", arg);

		break;
	}
	}

	return instruction_str;
}