#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H

#include <stdarg.h>
#include <stdint.h>

#define AM_ABS 		00		// Absolute
#define AM_AIX 		01		// Absolute indexed indirect 
#define AM_ABX 		02		// Absolute indexed X
#define AM_ABY 		03		// Absolute indexed Y
#define AM_AIN		04		// Absolute indirect
#define AM_A__	 	05		// Accumulator
#define AM_IMM	 	06		// Immediate
#define AM_IMP	 	07		// Implied
#define AM_REL	 	08		// Relative
#define AM_STK		09		// Stack
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

static const char *instruction_addr_mode_lookup[] = {
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

uint8_t print_args(uint8_t opcode, ...);

#endif