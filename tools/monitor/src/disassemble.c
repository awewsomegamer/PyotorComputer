#include "include/disassemble.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label *labels;
size_t label_count = 0;
uint16_t code_org = 0;
int pc = 0;

// Flags
// 0 0 0 0 0 S O L
//	     | | `-- Last call to this function returned a label (1: yes, 0: no)
//	     | `---- Use origin (1: yes, 0: no)
//	     `------ Single mode disassembly (1: yes, 0: no)

char *print_instruction(uint8_t *buffer, uint8_t *flags) {
	// Make this size dynamic
	char *instruction_str = malloc(512);

	uint8_t opcode = *(buffer + pc);
	uint8_t a = *(buffer + pc + 1);
	uint8_t b = *(buffer + pc + 2);
	uint16_t word = a | (b << 8);

	int label_index = -1;
	for (int i = 0; i < label_count; i++) {
		if ((pc + ((*flags >> 1) & 1 ? code_org : 0)) == labels[i].address && (*flags & 1) == 0) {
			*flags |= 1;
			label_index = i;
			goto return_label;
		}

		if (labels[i].address == word || labels[i].address == (pc + (int8_t)a))
			label_index = i;
	}

	*flags &= ~(1 << 0);

	sprintf(instruction_str, "%s", instruction_name_lookup[opcode]);

	switch (instruction_addr_mode_lookup[opcode]) {
	case AM_ABS: {
		if (label_index != -1) {
			sprintf(instruction_str + 3, " %s", labels[label_index].name);
			break;
		}

		sprintf(instruction_str + 3, " $%04X", word);

		break;
	}
	
	case AM_AIX: {
		if (label_index != -1) {
			sprintf(instruction_str + 3, " (%s, X)", labels[label_index].name);
			break;
		}

		sprintf(instruction_str + 3, " (%04X, X)", word);

		break;
	}

	case AM_ABX: {
		if (label_index != -1) {
			sprintf(instruction_str + 3, " %s, X", labels[label_index].name);
			break;
		}

		sprintf(instruction_str + 3, " %04X, X", word);

		break;
	}

	case AM_ABY: {
		if (label_index != -1) {
			sprintf(instruction_str + 3, " %s, Y", labels[label_index].name);
			break;
		}

		sprintf(instruction_str + 3, " %04X, Y", word);

		break;
	}

	case AM_AIN: {
		if (label_index != -1) {
			sprintf(instruction_str + 3, " (%s)", labels[label_index].name);
			break;
		}

		sprintf(instruction_str + 3, " (%04X)", word);

		break;
	}

	case AM_A__: {
		sprintf(instruction_str + 3, " A");

		break;
	}

	case AM_IMM: {
		if (label_index != -1) {
			sprintf(instruction_str + 3, " #%s", labels[label_index].name);
			break;
		}

		sprintf(instruction_str + 3, " #$%02X", a);

		break;
	}

	case AM_IMP: {
		break;
	}

	case AM_REL: {
		if (label_index != -1) {
			sprintf(instruction_str + 3, " %s", labels[label_index].name);
			break;
		}

		sprintf(instruction_str + 3, " $%02X", a);

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
		sprintf(instruction_str + 3, " ($%02X, X)", a);

		break;
	}

	case AM_ZPX: {
		sprintf(instruction_str + 3, " $%02X, X", a);

		break;
	}

	case AM_ZPY: {
		sprintf(instruction_str + 3, " $%02X, Y", a);

		break;
	}

	case AM_IND: {
		sprintf(instruction_str + 3, " ($%02X)", a);

		break;
	}

	case AM_INY: {
		sprintf(instruction_str + 3, " ($%02X), Y", a);

		break;
	}
	}

	// If not in single dissassembly mode, increment PC
	if (!((*flags >> 2) & 1))
		pc += instruction_size_lookup[instruction_addr_mode_lookup[opcode]];

	return instruction_str;

	return_label:;

	strcpy(instruction_str, labels[label_index].name);

	return instruction_str;
}

void parse_labels(FILE *file) {
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

		labels[label_count].name = name;
		labels[label_count].address = address;

		labels = (struct label *)realloc(labels, (++label_count + 1) * sizeof(struct label));
	}
}