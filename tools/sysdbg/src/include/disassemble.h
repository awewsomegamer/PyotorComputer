#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H

#include <stdarg.h>
#include <stdint.h>
#include "global.h"
#include "shared_memory.h"

#define AM_ABS 		0		// Absolute
#define AM_AIX 		1		// Absolute indexed indirect 
#define AM_ABX 		2		// Absolute indexed X
#define AM_ABY 		3		// Absolute indexed Y
#define AM_AIN		4		// Absolute indirect
#define AM_A__	 	5		// Accumulator
#define AM_IMM	 	6		// Immediate
#define AM_IMP	 	7		// Implied
#define AM_REL	 	8		// Relative
#define AM_STK		9		// Stack
#define AM_ZP_ 		10		// Zero page
#define AM_INX 		11		// Zero page indexed indirect
#define AM_ZPX 		12		// Zero page indexed X
#define AM_ZPY 		13		// Zero page indexed y
#define AM_IND 		14		// Zero page indirect
#define AM_INY 		15		// Zero page 

static const char *instruction_name_lookup[] = {
	"BRK", "ORA", "NOP", "NOP", "TSB", "ORA", "ASL", "RMB0", "PHP", "ORA", "ASL", "NOP", "TSB", "ORA", "ASL", "BBR0",
	"BPL", "ORA", "ORA", "NOP", "TRB", "ORA", "ASL", "RMB1", "CLC", "ORA", "INC", "NOP", "TRB", "ORA", "ASL", "BBR1",
	"JSR", "AND", "NOP", "NOP", "BIT", "AND", "ROL", "RMB2", "PLP", "AND", "ROL", "NOP", "BIT", "AND", "ROL", "BBR2",
	"BMI", "AND", "AND", "NOP", "BIT", "AND", "ROL", "RMB3", "SEC", "AND", "DEC", "NOP", "BIT", "AND", "ROL", "BBR3",
	"RTI", "EOR", "NOP", "NOP", "NOP", "EOR", "LSR", "RMB4", "PHA", "EOR", "LSR", "NOP", "JMP", "EOR", "LSR", "BBR4",
	"BVC", "EOR", "EOR", "NOP", "NOP", "EOR", "LSR", "RMB5", "CLI", "EOR", "PHY", "NOP", "NOP", "EOR", "LSR", "BBR5",
	"RTS", "ADC", "NOP", "NOP", "STZ", "ADC", "ROR", "RMB6", "PLA", "ADC", "ROR", "NOP", "JMP", "ADC", "ROR", "BBR6",
	"BVS", "ADC", "ADC", "NOP", "STZ", "ADC", "ROR", "RMB7", "SEI", "ADC", "PLY", "NOP", "JMP", "ADC", "ROR", "BBR7",
	"BRA", "STA", "NOP", "NOP", "STY", "STA", "STX", "SMB0", "DEY", "BIT", "TXA", "NOP", "STY", "STA", "STX", "BBS0",
	"BCC", "STA", "STA", "NOP", "STY", "STA", "STX", "SMB1", "TYA", "STA", "TXS", "NOP", "STZ", "STA", "STZ", "BBS1",
	"LDY", "LDA", "LDX", "NOP", "LDY", "LDA", "LDX", "SMB2", "TAY", "LDA", "TAX", "NOP", "LDY", "LDA", "LDX", "BBS2",
	"BCS", "LDA", "LDA", "NOP", "LDY", "LDA", "LDX", "SMB3", "CLV", "LDA", "TSX", "NOP", "LDY", "LDA", "LDX", "BBS3",
	"CPY", "CMP", "NOP", "NOP", "CPY", "CMP", "DEC", "SMB4", "INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "BBS4",
	"BNE", "CMP", "CMP", "NOP", "NOP", "CMP", "DEC", "SMB5", "CLD", "CMP", "PHX", "STP", "NOP", "CMP", "DEC", "BBS5",
	"CPX", "SBC", "NOP", "NOP", "CPX", "SBC", "INC", "SMB6", "INX", "SBC", "NOP", "NOP", "CPX", "SBC", "INC", "BBS6",
	"BEQ", "SBC", "SBC", "NOP", "NOP", "SBC", "INC", "SMB7", "SED", "SBC", "PLX", "NOP", "NOP", "SBC", "INC", "BBS7"
};

static const char instruction_addr_mode_lookup[] = {
	AM_STK, AM_INX, AM_IMP, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_ZP_, AM_STK, AM_IMM, AM_A__, AM_IMP, AM_ABS, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_ZP_, AM_ZPX, AM_ZPX, AM_ZP_, AM_IMP, AM_ABY, AM_A__, AM_IMP, AM_ABS, AM_ABX, AM_ABX, AM_REL,
	AM_ABS, AM_INX, AM_IMP, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_ZP_, AM_STK, AM_IMM, AM_A__, AM_IMP, AM_ABS, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_ZPX, AM_ZPX, AM_ZPX, AM_ZP_, AM_IMP, AM_ABY, AM_A__, AM_IMP, AM_ABX, AM_ABX, AM_ABX, AM_REL,
	AM_STK, AM_INX, AM_IMP, AM_IMP, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_STK, AM_IMM, AM_A__, AM_IMP, AM_ABS, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_IMP, AM_ZPX, AM_ZPX, AM_ZP_, AM_IMP, AM_ABY, AM_STK, AM_IMP, AM_IMP, AM_ABX, AM_ABX, AM_REL,
	AM_STK, AM_INX, AM_IMP, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_ZP_, AM_STK, AM_IMM, AM_A__, AM_IMP, AM_AIN, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_ZPX, AM_ZPX, AM_ZPX, AM_ZP_, AM_IMP, AM_ABY, AM_STK, AM_IMP, AM_AIX, AM_ABX, AM_ABX, AM_REL,
	AM_REL, AM_INX, AM_IMP, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_ZP_, AM_IMP, AM_IMM, AM_IMP, AM_IMP, AM_ABS, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_ZPX, AM_ZPX, AM_ZPY, AM_ZP_, AM_IMP, AM_ABY, AM_IMP, AM_IMP, AM_ABS, AM_ABX, AM_ABX, AM_REL,
	AM_IMM, AM_INX, AM_IMM, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_ZP_, AM_IMP, AM_IMM, AM_IMP, AM_IMP, AM_ABS, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_ZPX, AM_ZPX, AM_ZPY, AM_ZP_, AM_IMP, AM_ABY, AM_IMP, AM_IMP, AM_ABX, AM_ABX, AM_ABY, AM_REL,
	AM_IMM, AM_INX, AM_IMP, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_ZP_, AM_IMP, AM_IMM, AM_IMP, AM_IMP, AM_ABS, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_IMP, AM_ZPX, AM_ZPX, AM_ZP_, AM_IMP, AM_ABY, AM_STK, AM_IMP, AM_IMP, AM_ABX, AM_ABX, AM_REL,
	AM_IMM, AM_INX, AM_IMP, AM_IMP, AM_ZP_, AM_ZP_, AM_ZP_, AM_ZP_, AM_IMP, AM_IMM, AM_IMP, AM_IMP, AM_ABS, AM_ABS, AM_ABS, AM_REL,
	AM_REL, AM_INY, AM_IND, AM_IMP, AM_IMP, AM_ZPX, AM_ZPX, AM_ZP_, AM_IMP, AM_ABY, AM_STK, AM_IMP, AM_IMP, AM_ABX, AM_ABX, AM_REL,
};

static const int instruction_size_lookup[] = {
	[AM_ABS] = 3,
	[AM_AIX] = 3,
	[AM_ABX] = 3,
	[AM_ABY] = 3,
	[AM_AIN] = 3,
	[AM_A__] = 1,
	[AM_IMM] = 2,
	[AM_IMP] = 1,
	[AM_REL] = 2,
	[AM_STK] = 1,
	[AM_ZP_] = 2,
	[AM_INX] = 2,
	[AM_ZPX] = 2,
	[AM_ZPY] = 2,
	[AM_IND] = 2,
	[AM_INY] = 2
};

struct label {
        char *name;
        uint16_t address;
	uint8_t attributes; // 0 0 0 0 0 0 0 B
			    //	             `- Marked as breakpoint
};
extern struct label *labels;


extern int pc;
extern uint16_t code_org;

char *print_instruction(uint8_t *buffer, uint8_t *flags);
void parse_labels(FILE *file);
char *print_label();
int toggle_breakpoint(char *symbol);

#endif