#include "include/disassemble.h"
#include "include/global.h"
#include "include/shared_memory.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label *labels;
size_t label_count = 0;
uint16_t code_org = 0;
uint8_t disasm_flags = 0;
int pc = 0;

#define PAGE_SIZE 0x100
#define KB_16 0x3E80

enum memory_map {
        ZERO_PAGE_BASE = 0x0000,
        STACK_BASE = 0x0100,
        PROGRAM_MEM_BASE = 0x0200,
        KERNAL_DAT_BASE = PROGRAM_MEM_BASE + (KB_16 * 3),
        KERNAL_MEM_BASE = KERNAL_DAT_BASE + (KB_16 / 2),
        MEM_TOP = UINT16_MAX
};

#define PROGRAM_MEM_SZ (KB_16 * 3)
#define PROGRAM_MEM(a) (PROGRAM_MEM_BASE <= a && a <= KERNAL_DAT_BASE - 1)

uint8_t mem_byte_read(uint16_t address) {
        if (PROGRAM_MEM(address)) {
                uint16_t offset = (address - PROGRAM_MEM_BASE);
                uint8_t bank_bucket = offset > PROGRAM_MEM_SZ / 2 ? 1 : 0;
                uint8_t bank = (bank_bucket ? ((*memory >> 4) & 0b1111) : ((*memory) & 0b1111));

                if (bank == 0)
                        return *(memory + address);

                int base = PROGRAM_MEM_SZ * (bank - 1);

                return *((bank_bucket ? (memory + BANK_A_OFF) : (memory + BANK_B_OFF)) + (offset % PROGRAM_MEM_SZ / 2) + base);
        }

        return *(memory + address);
}

// Flags
// 0 0 0 B R S O L
//	 | | | | `-- Last call to this function returned a label (1: yes, 0: no)
//	 | | | `---- Use origin (1: yes, 0: no)
//	 | | `------ Single mode disassembly (1: yes, 0: no)
//	 | `-------- Buffer is Shared RAM
//	 `---------- Reached a breakpoint
char *print_instruction(uint8_t *buffer) {
	// Make this size dynamic
	char *instruction_str = malloc(512);

	uint8_t opcode = ((disasm_flags >> DISASM_FLAG_RAM) & 1) ? mem_byte_read(pc) : *(buffer + pc);
	uint8_t a = 	 ((disasm_flags >> DISASM_FLAG_RAM) & 1) ? mem_byte_read(pc + 1) : *(buffer + pc + 1);
	uint8_t b = 	 ((disasm_flags >> DISASM_FLAG_RAM) & 1) ? mem_byte_read(pc + 2) : *(buffer + pc + 2);
	uint16_t word = a | (b << 8);

	int label_index = -1;
	for (int i = 0; i < label_count; i++) {
		int pc_offset = (pc + ((disasm_flags >> DISASM_FLAG_ORG) & 1 ? code_org : 0));
		if (pc_offset == labels[i].address && ((disasm_flags >> DISASM_FLAG_LABEL) & 1) == 0) {
			disasm_flags |= 1 << DISASM_FLAG_LABEL;
			label_index = i;
			goto return_label;
		}

		if (labels[i].address == word || labels[i].address == (pc_offset + (int8_t)a))
			label_index = i;
	}

	disasm_flags &= ~(1 << 0);

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
	if (!((disasm_flags >> DISASM_FLAG_SINGLE) & 1))
		pc += instruction_size_lookup[instruction_addr_mode_lookup[opcode]];

	return instruction_str;

	return_label:;

	strcpy(instruction_str, labels[label_index].name);
	
	if (((labels[label_index].attributes >> LABEL_ATTR_BREAK) & 1) && ((disasm_flags >> DISASM_FLAG_BREAK) & 1) == 0) {
		*(uint64_t *)(memory + IPS_OFF) = 0;
		disasm_flags |= 1 << DISASM_FLAG_BREAK;
	}

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
		labels[label_count].attributes = 0;

		labels = (struct label *)realloc(labels, (++label_count + 1) * sizeof(struct label));
	}
}

// Same as parse_labels, except breakpoint attribute is enabled
void parse_breakpoints(FILE *file) {

}

int toggle_breakpoint(char *symbol) {
	int label_index = -1;
	for (int i = 0; i < label_count; i++) {
		if (strcmp(labels[i].name, symbol) == 0) {
			label_index = i;
			break;
		}
	}

	if (label_index == -1)
		return -1;

	if ((labels[label_index].attributes >> LABEL_ATTR_BREAK) & 1)
		labels[label_index].attributes &= ~(1 << LABEL_ATTR_BREAK);
	else
		labels[label_index].attributes |= 1 << LABEL_ATTR_BREAK;

	return (labels[label_index].attributes >> LABEL_ATTR_BREAK);
}