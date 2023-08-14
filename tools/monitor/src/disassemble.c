#include "include/disassemble.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label *labels;
size_t label_count = 0;
int pc = 0;

char *print_instruction(uint8_t *buffer) {
	char *instruction_str = malloc(3);

	uint8_t opcode = *(buffer + pc);

	sprintf(instruction_str, "%s", instruction_name_lookup[opcode]);

	uint8_t a = *(buffer + pc + 1);
	uint8_t b = (*(buffer + pc + 2) << 8);
	uint16_t word = a | (b << 8);

	instruction_str = realloc(instruction_str, 8);

	switch (instruction_addr_mode_lookup[opcode]) {
	case AM_ABS: {
		// Implement label resolution here
		sprintf(instruction_str + 3, " %04X", word);

		break;
	}
	
	case AM_AIX: {
		sprintf(instruction_str + 3, " (%04X, X)", word);

		break;
	}

	case AM_ABX: {
		sprintf(instruction_str + 3, " %04X, X", word);

		break;
	}

	case AM_ABY: {
		sprintf(instruction_str + 3, " %04X, Y", word);

		break;
	}

	case AM_AIN: {
		sprintf(instruction_str + 3, " (%04X)", word);

		break;
	}

	case AM_A__: {
		sprintf(instruction_str + 3, " A");

		break;
	}

	case AM_IMM: {
		sprintf(instruction_str + 3, " %02X", a);

		break;
	}

	case AM_IMP: {
		break;
	}

	case AM_REL: {
		sprintf(instruction_str + 3, " %02X", a);

		break;
	}

	case AM_STK: {
		break;
	}

	case AM_ZP_: {
		sprintf(instruction_str + 3, " %02X", a);

		break;
	}

	case AM_INX: {
		sprintf(instruction_str + 3, " (%02X, X)", a);

		break;
	}

	case AM_ZPX: {
		sprintf(instruction_str + 3, " %02X, X", a);

		break;
	}

	case AM_ZPY: {
		sprintf(instruction_str + 3, " %02X, Y", a);

		break;
	}

	case AM_IND: {
		sprintf(instruction_str + 3, " (%02X)", a);

		break;
	}

	case AM_INY: {
		sprintf(instruction_str + 3, " (%02X), Y", a);

		break;
	}
	}

	pc += instruction_size_lookup[instruction_addr_mode_lookup[opcode]];

	return instruction_str;
}

uint64_t hash_address(uint16_t address) {
	uint8_t a = address & 0xFF;
	uint8_t b = (address >> 8) & 0xFF;

	uint64_t h = (h * 256 + a) % label_count;
	h = (h * 256 + b) % label_count;

	return h;
}

void parse_labels(FILE *file) {
	fseek(file, 0, SEEK_SET);
	char c = 0;
	
	while ((c = fgetc(file)) != EOF)
		if (c == '\n')
			label_count++;

	fseek(file, 0, SEEK_SET);
	char *line = NULL;
	size_t len = 0;
	size_t size = 0;

	labels = (struct label *)malloc(sizeof(struct label));

	while ((size = getline(&line, &len, file) != -1)) {
		if ((*line) != 'a' && (*line + 1) != 'l')
			continue;

		uint16_t address = (uint16_t)strtol(line + 3, &line + 8, 16);
		char *name = (char *)malloc(len - 11);
		strcpy(name, line + 11);
		name[strlen(name) - 1] = 0;

		uint64_t index = hash_address(address);

		labels[index].name = name;
		labels[index].address = address;
	}
}

char *print_label() {

	return NULL;
}