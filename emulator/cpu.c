#include <cpu.h>
#include <ram.h>

// ** TODO: Implement BCD mode
// Initial states of registers
uint8_t register_a = 0;
uint8_t register_x = 0;
uint8_t register_y = 0;
uint8_t register_s = 0xFF;
struct {
        uint8_t C : 1;
        uint8_t Z : 1;
        uint8_t I : 1;
        uint8_t D : 1;
        uint8_t B : 1;
        uint8_t unused : 1;
        uint8_t V : 1;
        uint8_t N : 1;
} register_p;
uint16_t pc = 0;

// Pins
uint8_t pin_IRQ = 1;
uint8_t pin_NMI = 1;
uint8_t pin_RES = 1;

uint8_t waiting = 0;
uint8_t stopped = 0;

// Clock
uint64_t cycle_count = 0;

// Tables
void (*instruction[0xFF])();
//                       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
uint8_t cycles[256] = {  7, 6, 0, 0, 5, 3, 5, 0, 0, 2, 2, 0, 6, 4, 6, 0,   // 0
                         2, 5, 5, 0, 5, 4, 6, 0, 2, 4, 2, 0, 6, 4, 7, 0,   // 1
                         6, 6, 0, 0, 3, 3, 5, 0, 0, 2, 2, 0, 4, 4, 6, 0,   // 2
                         2, 5, 5, 0, 4, 4, 6, 0, 2, 4, 2, 0, 4, 4, 7, 0,   // 3
                         6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,   // 4
                         2, 5, 5, 0, 0, 4, 6, 0, 2, 4, 3, 0, 0, 4, 7, 0,   // 5
                         6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,   // 6
                         2, 5, 5, 0, 4, 4, 6, 0, 2, 4, 4, 0, 6, 4, 7, 0,   // 7
                         3, 6, 0, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,   // 8
                         2, 6, 5, 0, 4, 4, 4, 0, 2, 5, 2, 0, 4, 4, 5, 0,   // 9
                         2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,   // A
                         2, 5, 5, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,   // B
                         2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,   // C
                         2, 5, 5, 0, 0, 4, 6, 0, 2, 4, 3, 0, 0, 4, 7, 0,   // D
                         2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,   // E
                         2, 5, 5, 0, 0, 4, 6, 0, 2, 4, 4, 0, 0, 4, 7, 0 }; // F

// Run-time instructions
void INST_PHP();
void INST_BBS(int index) {
        cycle_count += 2;
        if ((PTR(NEXT_BYTE) >> index) & 1 == 1)
                pc = NEXT_WORD;
}

void INST_BBR(int index) {
        cycle_count += 2;
        if ((PTR(NEXT_BYTE) >> index) & 1 == 0)
                pc = NEXT_WORD;
}

void INST_SMB(int index) { mem_byte_write(PTR(CUR_BYTE) | (1 << index), NEXT_BYTE); }
void INST_RMB(int index) { mem_byte_write(PTR(CUR_BYTE) & ~(1 << index), NEXT_BYTE); }

void INST_BIT(uint8_t opcode) { 
        uint8_t value = 0;
        switch (PTR(pc - 1)) {
        case 0x24: // ZPG
                value = PTR(NEXT_BYTE);
                break;
        case 0x2C: // ABS
                value = PTR(NEXT_BYTE);
                break;
        case 0x89: // IMM
                value = NEXT_BYTE;
                break;
        case 0x34: // ZP,X
                value = PTR(OFF_X(NEXT_BYTE));
                break;
        case 0x3C: // ABS,X
                value = PTR(OFF_Y(NEXT_WORD));
                break;
        }

        uint8_t result = value & register_a;
        register_p.Z = (result == 0);

        if (opcode != 0x89) { // Apparently IMM does not set these flags?
                register_p.N = (result >> 7) & 1;
                register_p.V = (result >> 6) & 1;
        }
}

void call_interrupt() {
        mem_byte_write(((pc + 2) >> 8) & 0xFF, 0x100 + (register_s--)); // High
        mem_byte_write((pc + 2) & 0xFF, 0x100 + (register_s--)); // Low
        INST_PHP(); // Push flags
        register_p.I = 1; // Disable interrupts

        if (!pin_NMI) {
                // NMI
                pc = PTR(0xFFFA) | (PTR(0xFFFB) << 8);
                pin_NMI = 1;
        } else if (!pin_IRQ) {
                // IRQ
                pc = PTR(0xFFFE) | (PTR(0xFFFF) << 8);
                pin_IRQ = 1;
        } else if (!pin_RES) {
                // Reset
                pc = PTR(0xFFFC) | (PTR(0xFFFD) << 8);
                pin_RES = 1;
        }
}

// Preform one cycle on the 6502
void tick_65C02() {
        if ((waiting && (pin_IRQ && pin_NMI && pin_RES)) || (stopped && pin_RES)) {
                // We are waiting
                cycle_count++; // Should this be done this way?
                return;
        }

        // printf("A\n");

        if ((!pin_IRQ && !register_p.I) || !pin_NMI || !pin_RES) // NMI, RES, or IRQ pins went low, check if IRQs are unmasked
                call_interrupt();

        uint8_t opcode = NEXT_BYTE;
        uint8_t high_nibble = ((opcode >> 4) & 0xF);
        uint8_t low_nibble = (opcode & 0xF);

        // BBSs
        if (high_nibble >= 0x8 &&  high_nibble <= 0xF && low_nibble == 0xF) {
                INST_BBS((high_nibble) - 8); 
                goto TICK_RET;
        }

        // BBRs
        if (high_nibble >= 0x0 &&  high_nibble <= 7 && low_nibble == 0xF) {
                INST_BBR(high_nibble); 
                goto TICK_RET;
        }

        // SMBs
        if (high_nibble >= 0x8 &&  high_nibble <= 0xF && low_nibble == 0x7) {
                INST_SMB((high_nibble) - 8); 
                goto TICK_RET;
        }

        // RMBs
        if (high_nibble >= 0x0 &&  high_nibble <= 0x7 && low_nibble == 0x7) {
                INST_RMB(high_nibble); 
                goto TICK_RET;
        }

        // BITs
        if (opcode == 0x24 || opcode == 0x2C || opcode == 0x89 || opcode == 0x34 || opcode == 0x3C) {
                INST_BIT(opcode);
                goto TICK_RET;
        }

        // Consult instruction table
        (*instruction[opcode])();

        TICK_RET:
        cycle_count += cycles[opcode];
}

uint8_t ARIT_LEFT_SHIFT(uint8_t what) {
        uint8_t result = (what << 1) | register_p.C;
        SET_NZ(result)
        register_p.C = (what >> 7) & 1;
        return result;
}

uint8_t ARIT_RIGHT_SHIFT(uint8_t what) {
        uint8_t result = (what >> 1) | (register_p.C << 7);
        SET_NZ(result)
        register_p.C = (what & 1);
        return result;
}

// LD instructions
void INST_LDA_IMM() { register_a = NEXT_BYTE; SET_NZ(register_a) }
void INST_LDX_IMM() { register_x = NEXT_BYTE; SET_NZ(register_x) }
void INST_LDY_IMM() { register_y = NEXT_BYTE; SET_NZ(register_y) }
void INST_LDA_ABS() { register_a = PTR(NEXT_WORD); SET_NZ(register_a) }
void INST_LDX_ABS() { register_x = PTR(NEXT_WORD); SET_NZ(register_x) }
void INST_LDY_ABS() { register_y = PTR(NEXT_WORD); SET_NZ(register_y) }
void INST_LDA_ZPG() { register_a = PTR(NEXT_BYTE); SET_NZ(register_a) }
void INST_LDX_ZPG() { register_x = PTR(NEXT_BYTE); SET_NZ(register_x) }
void INST_LDY_ZPG() { register_y = PTR(NEXT_BYTE); SET_NZ(register_y) }
void INST_LDA_ZPG_X() { register_a = PTR(OFF_X(NEXT_BYTE)); SET_NZ(register_a) }
void INST_LDX_ZPG_Y() { register_x = PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); SET_NZ(register_x) }
void INST_LDY_ZPG_X() { register_y = PTR(OFF_X(NEXT_BYTE)); SET_NZ(register_y) }
void INST_LDA_IND_X() { register_a = PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8)); SET_NZ(register_x)}
void INST_LDA_IND_Y() { register_a = PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); SET_NZ(register_y) }
void INST_LDA_ABS_X() { register_a = PTR(OFF_X(NEXT_WORD)); SET_NZ(register_x) }
void INST_LDA_ABS_Y() { register_a = PTR(OFF_Y(NEXT_WORD)); SET_NZ(register_y) }
void INST_LDY_ABS_X() { register_y = PTR(OFF_X(NEXT_WORD)); SET_NZ(register_x) }
void INST_LDX_ABS_Y() { register_x = PTR(OFF_Y(NEXT_WORD)); SET_NZ(register_y) }
void INST_LDA_ZPG_IND() { register_a = PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)); }

// ST instructions
void INST_STA_ABS() { mem_byte_write(register_a, NEXT_WORD); }
void INST_STX_ABS() { mem_byte_write(register_x, NEXT_WORD); }
void INST_STY_ABS() { mem_byte_write(register_y, NEXT_WORD); }
void INST_STA_ZPG() { mem_byte_write(register_a, NEXT_BYTE); }
void INST_STX_ZPG() { mem_byte_write(register_x, NEXT_BYTE); }
void INST_STY_ZPG() { mem_byte_write(register_y, NEXT_BYTE); }
void INST_STA_ZPG_X() { mem_byte_write(register_a, OFF_X(NEXT_BYTE)); }
void INST_STX_ZPG_Y() { mem_byte_write(register_x, OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }
void INST_STY_ZPG_X() { mem_byte_write(register_y, OFF_X(NEXT_BYTE)); }
void INST_STA_IND_X() { mem_byte_write(register_a, PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8)); }
void INST_STA_IND_Y() { mem_byte_write(register_a, OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }
void INST_STA_ABS_X() { mem_byte_write(register_a, OFF_X(NEXT_WORD)); }
void INST_STA_ABS_Y() { mem_byte_write(register_a, OFF_Y(NEXT_WORD)); }

void INST_STA_ZPG_IND() { mem_byte_write(register_a, (PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }
void INST_STZ_ZPG() { mem_byte_write(0x00, NEXT_BYTE); }
void INST_STZ_ZPG_X() { mem_byte_write(0x00, OFF_X(NEXT_BYTE)); }
void INST_STZ_ABS() { mem_byte_write(0x00, NEXT_WORD); }
void INST_STZ_ABS_X() { mem_byte_write(0x00, OFF_X(NEXT_WORD)); }

// Stack instructions
void INST_PHP() { mem_byte_write(*(uint8_t *)&register_p, 0x100 + (register_s--)); } // PUSH P
void INST_PLP() { *(uint8_t *)&register_p = PTR(0x100 + (++register_s)); } // PULL P
void INST_PHA() { mem_byte_write(register_a, 0x100 + (register_s--)); } // PUSH A
void INST_PLA() { register_a = PTR(0x100 + (++register_s)); SET_NZ(register_a)} // PULL A
void INST_PHX() { mem_byte_write(register_x, 0x100 + (register_s--)); } // PUSH X
void INST_PLX() { register_x = PTR(0x100 + (++register_s)); SET_NZ(register_x)} // PULL X
void INST_PHY() { mem_byte_write(register_y, 0x100 + (register_s--)); } // PUSH Y
void INST_PLY() { register_y = PTR(0x100 + (++register_s)); SET_NZ(register_y)} // PULL Y

// P register (Flag) instructions
void INST_SED() { register_p.D = 1; }
void INST_CLD() { register_p.D = 0; }
void INST_CLV() { register_p.V = 0; }
void INST_SEI() { register_p.I = 1; }
void INST_CLI() { register_p.I = 0; }
void INST_SEC() { register_p.C = 1; }
void INST_CLC() { register_p.C = 0; }

// Increment / Decrement instructions
void INST_INA() { register_a++; SET_NZ(register_a) }
void INST_DEA() { register_a--; SET_NZ(register_a) }
void INST_INX() { register_x++; SET_NZ(register_x) }
void INST_DEX() { register_x--; SET_NZ(register_x) }
void INST_INY() { register_y++; SET_NZ(register_y) }
void INST_DEY() { register_y--; SET_NZ(register_y) }
void INST_INC_ABS() { uint16_t addr = NEXT_WORD; mem_byte_write(PTR(addr) + 1, addr); SET_NZ(PTR(addr)) }
void INST_DEC_ABS() { uint16_t addr = NEXT_WORD; mem_byte_write(PTR(addr) - 1, addr); SET_NZ(PTR(addr)) }
void INST_DEC_ZPG() { uint16_t addr = NEXT_BYTE; mem_byte_write(PTR(addr) - 1, addr); SET_NZ(PTR(addr)) }
void INST_DEC_ZPG_X() { uint16_t addr = OFF_X(NEXT_BYTE); mem_byte_write(PTR(addr) - 1, addr); SET_NZ(PTR(addr)) }
void INST_INC_ZPG() { uint16_t addr = NEXT_BYTE; mem_byte_write(PTR(addr) + 1, addr); SET_NZ(PTR(addr)) }
void INST_INC_ZPG_X() { uint16_t addr = OFF_X(NEXT_BYTE); mem_byte_write(PTR(addr) + 1, addr); SET_NZ(PTR(addr)) }
void INST_DEC_ABS_X() { uint16_t addr = OFF_X(NEXT_WORD); mem_byte_write(PTR(addr) - 1, addr); SET_NZ(PTR(addr)) }
void INST_INC_ABS_X() { uint16_t addr = OFF_X(NEXT_WORD); mem_byte_write(PTR(addr) + 1, addr); SET_NZ(PTR(addr)) }

// Transfer instructions
void INST_TYA() { register_a = register_y; SET_NZ(register_a) }
void INST_TAY() { register_y = register_a; SET_NZ(register_y) }
void INST_TXA() { register_a = register_x; SET_NZ(register_a) }
void INST_TXS() { register_s = register_x; }
void INST_TAX() { register_x = register_a; SET_NZ(register_x) }
void INST_TSX() { register_x = register_s; SET_NZ(register_x) }

// Control flow instructions
void INST_RTI() {
        // Return from interrupt
        INST_PLP();
        pc = PTR(0x100 + ++register_s) | (PTR(0x100 + ++register_s) << 8);
}

void INST_JSR() {
        mem_byte_write(((pc + 2) >> 8) & 0xFF, 0x100 + (register_s--));
        mem_byte_write((pc + 2) & 0xFF, 0x100 + (register_s--));
        pc = NEXT_WORD;
}

void INST_RTS() {
        // Return from sub-routine
        pc = PTR(0x100 + ++register_s) | (PTR(0x100 + ++register_s) << 8);
}

void INST_JMP_ABS() { pc = NEXT_WORD; }
void INST_JMP_IND() { pc = PTR(NEXT_WORD) | (PTR(NEXT_WORD) << 8); }
void INST_JMP_ABS_X() { pc = OFF_X(NEXT_WORD); }
void INST_BPL_REL() { if (!register_p.N) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BMI_REL() { if (register_p.N) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BVC_REL() { if (!register_p.V) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BVS_REL() { if (register_p.V) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BCC_REL() { if (!register_p.C) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BCS_REL() { if (register_p.C) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BNE_REL() { if (!register_p.Z) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BEQ_REL() { if (register_p.Z) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BRA_REL() { pc += (int8_t)NEXT_BYTE; }

// Logical instructions
void INST_ASL_A() { register_a = ARIT_LEFT_SHIFT(register_a); } // Shift the A register left (N: original bit in bit 6, Z: if the result is == 0, otherwise it is resets Z and stores the original bit 7 in the carry)
void INST_ROL_A() { register_a = ARIT_LEFT_SHIFT(register_a); } // Rotate the A register left 1, old carry overflows to bit 0, old bit 7 overflows to carry
void INST_LSR_A() { register_a = ARIT_RIGHT_SHIFT(register_a); register_p.N = 0;} // Logical Shift Right shift right by 1 bit 0 is shifted into bit 7 and original bit 0 overflows to carry
void INST_ROR_A() { register_a = ARIT_RIGHT_SHIFT(register_a); } // Rotate the A register right 1, old cary overflows to bit 7, old bit 0 overflows to carry

void INST_ORA_IND_X() { ARIT_OR_SET(register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_ORA_IND_Y() { ARIT_OR_SET(register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_ORA_ZPG_IND() { ARIT_OR_SET(register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_AND_IND_X() { ARIT_AND_SET(register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_AND_IND_Y() { ARIT_AND_SET(register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_EOR_IND_X() { ARIT_EOR_SET(register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_EOR_IND_Y() { ARIT_EOR_SET(register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_EOR_ZPG_IND() { ARIT_EOR_SET(register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }

void INST_TRB_ZPG() {
        uint8_t result = PTR(CUR_BYTE) & (register_a ^ 0xFF);
        register_p.Z = (PTR(CUR_BYTE) == 0);
        mem_byte_write(NEXT_BYTE, result);
}

void INST_TRB_ABS() {
        uint8_t result = PTR(CUR_WORD) & (register_a ^ 0xFF);
        register_p.Z = (PTR(CUR_WORD) == 0);
        mem_byte_write(NEXT_WORD, result);
}

void INST_TSB_ZPG() {
        uint8_t result = PTR(CUR_BYTE) | register_a;
        register_p.Z = (PTR(CUR_BYTE) == 0);
        mem_byte_write(NEXT_BYTE, result);
}

void INST_TSB_ABS() {
        uint8_t result = PTR(CUR_WORD) | register_a;
        register_p.Z = (PTR(CUR_WORD) == 0);
        mem_byte_write(NEXT_WORD, result);
}

void INST_ORA_ZPG() { ARIT_OR_SET(register_a, PTR(NEXT_BYTE)) }
void INST_ORA_ZPG_X() { ARIT_OR_SET(register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_AND_ZPG() { ARIT_AND_SET(register_a, PTR(NEXT_BYTE)) }
void INST_AND_ZPG_X() { ARIT_AND_SET(register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_AND_ZPG_IND() { ARIT_AND_SET(register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) } 
void INST_EOR_ZPG() { ARIT_EOR_SET(register_a, PTR(NEXT_BYTE)) }
void INST_EOR_ZPG_X() { ARIT_EOR_SET(register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_ASL_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ASL_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ROL_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(ARIT_RIGHT_SHIFT(PTR(address)), address); }
void INST_ROL_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(ARIT_RIGHT_SHIFT(PTR(address)), address); }
void INST_LSR_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); register_p.N = 0; }
void INST_LSR_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); register_p.N = 0; }
void INST_ROR_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(ARIT_RIGHT_SHIFT(PTR(address)), address); }
void INST_ROR_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(ARIT_RIGHT_SHIFT(PTR(address)), address); }
void INST_ORA_IMM() { ARIT_OR_SET(register_a, NEXT_BYTE) }
void INST_ORA_ABS_Y() { ARIT_OR_SET(register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_AND_IMM() { ARIT_AND_SET(register_a, NEXT_BYTE) }
void INST_AND_ABS_Y() { ARIT_AND_SET(register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_EOR_IMM() { ARIT_EOR_SET(register_a, NEXT_BYTE) }
void INST_EOR_ABS_Y() { ARIT_EOR_SET(register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_ORA_ABS() { ARIT_OR_SET(register_a, PTR(NEXT_WORD)) }
void INST_ORA_ABS_X() { ARIT_OR_SET(register_a, PTR(NEXT_WORD)) }
void INST_AND_ABS() { ARIT_AND_SET(register_a, PTR(NEXT_WORD)) }
void INST_AND_ABS_X() { ARIT_AND_SET(register_a, PTR(OFF_X(NEXT_WORD))) }
void INST_EOR_ABS() { ARIT_EOR_SET(register_a, PTR(NEXT_WORD)) }
void INST_EOR_ABS_X() { ARIT_EOR_SET(register_a, PTR(OFF_X(NEXT_WORD))) }
void INST_ASL_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ASL_ABS_X() { uint16_t address = OFF_X(NEXT_WORD); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ROL_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ROL_ABS_X() { uint16_t address = OFF_X(NEXT_WORD); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_LSR_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); register_p.N = 0; }
void INST_LSR_ABS_X() { uint16_t address = OFF_X(NEXT_WORD); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); register_p.N = 0; }
void INST_ROR_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ROR_ABS_X() { uint16_t address = OFF_X(NEXT_WORD); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }

// Arithmetic instructions
void INST_ADC_IND_X() { ARIT_ADD_SET(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8)) }
void INST_ADC_IND_Y() { ARIT_ADD_SET(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_ADC_ZPG_IND() { ARIT_ADD_SET(PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_SBC_IND_X() { ARIT_SUB_SET(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8)) }
void INST_SBC_IND_Y() { ARIT_SUB_SET(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_SBC_ZPG_IND() { ARIT_SUB_SET(PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_ADC_ZPG() { ARIT_ADD_SET(NEXT_BYTE) }
void INST_ADC_ZPG_X() { ARIT_ADD_SET(OFF_X(NEXT_BYTE)) }
void INST_SBC_ZPG() { ARIT_SUB_SET(NEXT_BYTE) }
void INST_SBC_ZPG_X() { ARIT_SUB_SET(OFF_X(NEXT_BYTE)) }
void INST_ADC_IMM() { ARIT_ADD_SET(NEXT_BYTE) }
void INST_ADC_ABS_Y() { ARIT_ADD_SET(OFF_Y(NEXT_WORD)) }
void INST_SBC_IMM() { ARIT_SUB_SET(NEXT_BYTE) }
void INST_SBC_ABS_Y() { ARIT_SUB_SET(OFF_Y(NEXT_WORD)) }
void INST_ADC_ABS() { ARIT_ADD_SET(NEXT_WORD) }
void INST_ADC_ABS_X() { ARIT_ADD_SET(OFF_X(NEXT_WORD)) }
void INST_SBC_ABS() { ARIT_SUB_SET(NEXT_WORD) }
void INST_SBC_ABS_X() { ARIT_SUB_SET(OFF_X(NEXT_WORD)) }

// Comparison instructions
void INST_CPX_IMM() { CMP_SET(register_x, NEXT_BYTE) }
void INST_CPY_IMM() { CMP_SET(register_y, NEXT_BYTE) }
void INST_CMP_IND_X() { CMP_SET(register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(CUR_BYTE + 1)) << 8))) }
void INST_CMP_IND_Y() { CMP_SET(register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE) << 8)))) }
void INST_CPX_ZPG() { CMP_SET(register_x, PTR(NEXT_BYTE)) }
void INST_CPY_ZPG() { CMP_SET(register_y, PTR(NEXT_BYTE)) }
void INST_CMP_ZPG() { CMP_SET(register_a, PTR(NEXT_BYTE)) }
void INST_CMP_ZPG_X() { CMP_SET(register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_CMP_IMM() { CMP_SET(register_a, NEXT_BYTE) }
void INST_CMP_ABS_Y() {CMP_SET(register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_CPX_ABS() { CMP_SET(register_x, PTR(NEXT_WORD)) }
void INST_CPY_ABS() { CMP_SET(register_y, PTR(NEXT_WORD)) }
void INST_CMP_ABS() { CMP_SET(register_a, PTR(NEXT_WORD)) }
void INST_CMP_ABS_X() { CMP_SET(register_a, PTR(OFF_X(NEXT_WORD))) }
void INST_CMP_ZPG_IND() { CMP_SET(register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }

// Porcessor instructions
void INST_BRK() {
        mem_byte_write(((pc + 1) >> 8) & 0xFF, 0x100 + (register_s--)); // High
        mem_byte_write((pc + 1) & 0xFF, 0x100 + (register_s--)); // Low
        INST_PHP(); // Push flags

        // Change flags
        register_p.B = 1; 
        register_p.I = 1;
        register_p.D = 0;

        pc = PTR(0xFFFE) | (PTR(0xFFFF) << 8);
}
void INST_WAI() { waiting = 1; }
void INST_STP() { stopped = 1; }

// Just NOP
void INST_NOP() { ; }

void reg_dump_65C02() {
        printf("-- 65C02 REG DUMP --\n");
        printf("A     : %02X (%d)\n", register_a, register_a);
        printf("X     : %02X (%d)\n", register_x, register_x);
        printf("Y     : %02X (%d)\n", register_y, register_y);
        printf("S     : %02X (%d)\n", register_s, register_s);
        printf("P     : %02X (", register_p, register_p);
        if (register_p.N) printf("N");
        if (register_p.V) printf(" | V");
        if (register_p.unused) printf(" | Unused");
        if (register_p.B) printf(" | B");
        if (register_p.D) printf(" | D");
        if (register_p.I) printf(" | I");
        if (register_p.Z) printf(" | Z");
        if (register_p.C) printf(" | C");
        printf(")\n");

        printf("PC    : %04X (%02X", pc, PTR(pc));
        if (pc + 1 < UINT16_MAX) printf(" %02X", PTR(pc + 1));
        if (pc + 2 < UINT16_MAX) printf(" %02X", PTR(pc + 2));
        printf(")\n");

        printf("CYCLE : %X (%d)\n", cycle_count, cycle_count);

        printf("-- 65C02 REG DUMP END --\n");
}

// Initialize the 6502
void init_65C02() {
        DBG(1, printf("Initializing 65C02 CPU");)

        *(uint8_t *)&register_p = 0b00110100;

        int installed = 0;

        // LD instructions
        instruction[0xA9] = INST_LDA_IMM; DBG(0, installed++;)
        instruction[0xA2] = INST_LDX_IMM; DBG(0, installed++;)
        instruction[0xA0] = INST_LDY_IMM; DBG(0, installed++;)
        instruction[0xAD] = INST_LDA_ABS; DBG(0, installed++;)
        instruction[0xAE] = INST_LDX_ABS; DBG(0, installed++;)
        instruction[0xAC] = INST_LDY_ABS; DBG(0, installed++;)
        instruction[0xA5] = INST_LDA_ZPG; DBG(0, installed++;)
        instruction[0xA6] = INST_LDX_ZPG; DBG(0, installed++;)
        instruction[0xA4] = INST_LDY_ZPG; DBG(0, installed++;)
        instruction[0xB5] = INST_LDA_ZPG_X; DBG(0, installed++;)
        instruction[0xB6] = INST_LDX_ZPG_Y; DBG(0, installed++;)
        instruction[0xB4] = INST_LDY_ZPG_X; DBG(0, installed++;)
        instruction[0xA1] = INST_LDA_IND_X; DBG(0, installed++;)
        instruction[0xB1] = INST_LDA_IND_Y; DBG(0, installed++;)
        instruction[0xBD] = INST_LDA_ABS_X; DBG(0, installed++;)
        instruction[0xB9] = INST_LDA_ABS_Y; DBG(0, installed++;)
        instruction[0xBC] = INST_LDY_ABS_X; DBG(0, installed++;)
        instruction[0xBE] = INST_LDX_ABS_Y; DBG(0, installed++;)
        instruction[0xB2] = INST_LDA_ZPG_IND; DBG(0, installed++;)

        // ST instructions
        instruction[0x8D] = INST_STA_ABS; DBG(0, installed++;)
        instruction[0x8E] = INST_STX_ABS; DBG(0, installed++;)
        instruction[0x8C] = INST_STY_ABS; DBG(0, installed++;)
        instruction[0x85] = INST_STA_ZPG; DBG(0, installed++;)
        instruction[0x86] = INST_STX_ZPG; DBG(0, installed++;)
        instruction[0x84] = INST_STY_ZPG; DBG(0, installed++;)
        instruction[0x95] = INST_STA_ZPG_X; DBG(0, installed++;)
        instruction[0x96] = INST_STX_ZPG_Y; DBG(0, installed++;)
        instruction[0x94] = INST_STY_ZPG_X; DBG(0, installed++;)
        instruction[0x81] = INST_STA_IND_X; DBG(0, installed++;)
        instruction[0x91] = INST_STA_IND_Y; DBG(0, installed++;)
        instruction[0x9D] = INST_STA_ABS_X; DBG(0, installed++;)
        instruction[0x99] = INST_STA_ABS_Y; DBG(0, installed++;)
        instruction[0x92] = INST_STA_ZPG_IND; DBG(0, installed++;)
        instruction[0x64] = INST_STZ_ZPG; DBG(0, installed++;)
        instruction[0x74] = INST_STZ_ZPG_X; DBG(0, installed++;)
        instruction[0x9C] = INST_STZ_ABS; DBG(0, installed++;)
        instruction[0x9E] = INST_STZ_ABS_X; DBG(0, installed++;)

        // Stack instructions
        instruction[0x08] = INST_PHP; DBG(0, installed++;)
        instruction[0x28] = INST_PLP; DBG(0, installed++;)
        instruction[0x48] = INST_PHA; DBG(0, installed++;)
        instruction[0x68] = INST_PLA; DBG(0, installed++;)
        instruction[0xDA] = INST_PHX; DBG(0, installed++;)
        instruction[0xFA] = INST_PLX; DBG(0, installed++;)
        instruction[0x5A] = INST_PHY; DBG(0, installed++;)
        instruction[0x7A] = INST_PLY; DBG(0, installed++;)

        // P register (Flag) instructions
        instruction[0xF8] = INST_SED; DBG(0, installed++;)
        instruction[0xD8] = INST_CLD; DBG(0, installed++;)
        instruction[0xB8] = INST_CLV; DBG(0, installed++;)
        instruction[0x78] = INST_SEI; DBG(0, installed++;)
        instruction[0x58] = INST_CLI; DBG(0, installed++;)
        instruction[0x38] = INST_SEC; DBG(0, installed++;)
        instruction[0x18] = INST_CLC; DBG(0, installed++;)

        // Increment / Decrement instructions
        instruction[0x1A] = INST_INA; DBG(0, installed++;)
        instruction[0x3A] = INST_DEA; DBG(0, installed++;)
        instruction[0xE8] = INST_INX; DBG(0, installed++;)
        instruction[0xCA] = INST_DEX; DBG(0, installed++;)
        instruction[0xC8] = INST_INY; DBG(0, installed++;)
        instruction[0x88] = INST_DEY; DBG(0, installed++;)
        instruction[0xEE] = INST_INC_ABS; DBG(0, installed++;)
        instruction[0xCE] = INST_DEC_ABS; DBG(0, installed++;)
        instruction[0xC6] = INST_DEC_ZPG; DBG(0, installed++;)
        instruction[0xD6] = INST_DEC_ZPG_X; DBG(0, installed++;)
        instruction[0xE6] = INST_INC_ZPG; DBG(0, installed++;)
        instruction[0xF6] = INST_INC_ZPG_X; DBG(0, installed++;)
        instruction[0xDE] = INST_DEC_ABS_X; DBG(0, installed++;)
        instruction[0xFE] = INST_INC_ABS_X; DBG(0, installed++;)

        // Transfer Instructions
        instruction[0x98] = INST_TYA; DBG(0, installed++;)
        instruction[0xA8] = INST_TAY; DBG(0, installed++;)
        instruction[0x8A] = INST_TXA; DBG(0, installed++;)
        instruction[0x9A] = INST_TXS; DBG(0, installed++;)
        instruction[0xAA] = INST_TAX; DBG(0, installed++;)
        instruction[0xBA] = INST_TSX; DBG(0, installed++;)

        // Control flow instructions
        instruction[0x40] = INST_RTI; DBG(0, installed++;)
        instruction[0x20] = INST_JSR; DBG(0, installed++;)
        instruction[0x60] = INST_RTS; DBG(0, installed++;)
        instruction[0x4C] = INST_JMP_ABS; DBG(0, installed++;)
        instruction[0x6C] = INST_JMP_IND; DBG(0, installed++;)
        instruction[0x10] = INST_BPL_REL; DBG(0, installed++;)
        instruction[0x30] = INST_BMI_REL; DBG(0, installed++;)
        instruction[0x50] = INST_BVC_REL; DBG(0, installed++;)
        instruction[0x70] = INST_BVS_REL; DBG(0, installed++;)
        instruction[0x90] = INST_BCC_REL; DBG(0, installed++;)
        instruction[0xB0] = INST_BCS_REL; DBG(0, installed++;)
        instruction[0xD0] = INST_BNE_REL; DBG(0, installed++;)
        instruction[0xF0] = INST_BEQ_REL; DBG(0, installed++;)
        instruction[0x7C] = INST_JMP_ABS_X; DBG(0, installed++;)
        instruction[0x80] = INST_BRA_REL; DBG(0, installed++;)

        // Compare instructions
        instruction[0xE0] = INST_CPX_IMM; DBG(0, installed++;)
        instruction[0xC0] = INST_CPY_IMM; DBG(0, installed++;)
        instruction[0xC1] = INST_CMP_IND_X; DBG(0, installed++;)
        instruction[0xE4] = INST_CPX_ZPG; DBG(0, installed++;)
        instruction[0xC4] = INST_CPY_ZPG; DBG(0, installed++;)
        instruction[0xC5] = INST_CMP_ZPG; DBG(0, installed++;)
        instruction[0xD1] = INST_CMP_IND_Y; DBG(0, installed++;)
        instruction[0xD5] = INST_CMP_ZPG_X; DBG(0, installed++;)
        instruction[0xC9] = INST_CMP_IMM; DBG(0, installed++;)
        instruction[0xD9] = INST_CMP_ABS_Y; DBG(0, installed++;)
        instruction[0xEC] = INST_CPX_ABS; DBG(0, installed++;)
        instruction[0xCC] = INST_CPY_ABS; DBG(0, installed++;)
        instruction[0xCD] = INST_CMP_ABS; DBG(0, installed++;)
        instruction[0xDD] = INST_CMP_ABS_X; DBG(0, installed++;)
        instruction[0xD2] = INST_CMP_ZPG_IND; DBG(0, installed++;)

        // Arithmetic instructions
        instruction[0x61] = INST_ADC_IND_X; DBG(0, installed++;)
        instruction[0x71] = INST_ADC_IND_Y; DBG(0, installed++;)
        instruction[0x72] = INST_ADC_ZPG_IND; DBG(0, installed++;)
        instruction[0xE1] = INST_SBC_IND_X; DBG(0, installed++;)
        instruction[0xF1] = INST_SBC_IND_Y; DBG(0, installed++;)
        instruction[0xF2] = INST_SBC_ZPG_IND; DBG(0, installed++;)
        instruction[0x65] = INST_ADC_ZPG; DBG(0, installed++;)
        instruction[0x75] = INST_ADC_ZPG_X; DBG(0, installed++;)
        instruction[0xE5] = INST_SBC_ZPG; DBG(0, installed++;)
        instruction[0xF5] = INST_SBC_ZPG_X; DBG(0, installed++;)
        instruction[0x69] = INST_ADC_IMM; DBG(0, installed++;)
        instruction[0x79] = INST_ADC_ABS_Y; DBG(0, installed++;)
        instruction[0xE9] = INST_SBC_IMM; DBG(0, installed++;)
        instruction[0xF9] = INST_SBC_ABS_Y; DBG(0, installed++;)
        instruction[0x6D] = INST_ADC_ABS; DBG(0, installed++;)
        instruction[0x7D] = INST_ADC_ABS_X; DBG(0, installed++;)
        instruction[0xED] = INST_SBC_ABS; DBG(0, installed++;)
        instruction[0xFD] = INST_SBC_ABS_X; DBG(0, installed++;)

        // Logical Instructions
        instruction[0x01] = INST_ORA_IND_X; DBG(0, installed++;)
        instruction[0x11] = INST_ORA_IND_Y; DBG(0, installed++;)
        instruction[0x12] = INST_ORA_ZPG_IND; DBG(0, installed++;)
        instruction[0x21] = INST_AND_IND_X; DBG(0, installed++;)
        instruction[0x31] = INST_AND_IND_Y; DBG(0, installed++;)
        instruction[0x32] = INST_AND_ZPG_IND; DBG(0, installed++;)
        instruction[0x41] = INST_EOR_IND_X; DBG(0, installed++;)
        instruction[0x51] = INST_EOR_IND_Y; DBG(0, installed++;)
        instruction[0x52] = INST_EOR_ZPG_IND; DBG(0, installed++;)
        instruction[0x05] = INST_ORA_ZPG; DBG(0, installed++;)
        instruction[0x15] = INST_ORA_ZPG_X; DBG(0, installed++;)
        instruction[0x25] = INST_AND_ZPG; DBG(0, installed++;)
        instruction[0x35] = INST_AND_ZPG_X; DBG(0, installed++;)
        instruction[0x45] = INST_EOR_ZPG; DBG(0, installed++;)
        instruction[0x55] = INST_EOR_ZPG_X; DBG(0, installed++;)
        instruction[0x06] = INST_ASL_ZPG; DBG(0, installed++;)
        instruction[0x16] = INST_ASL_ZPG_X; DBG(0, installed++;)
        instruction[0x26] = INST_ROL_ZPG; DBG(0, installed++;)
        instruction[0x36] = INST_ROL_ZPG_X; DBG(0, installed++;)
        instruction[0x46] = INST_LSR_ZPG; DBG(0, installed++;)
        instruction[0x56] = INST_LSR_ZPG_X; DBG(0, installed++;)
        instruction[0x66] = INST_ROR_ZPG; DBG(0, installed++;)
        instruction[0x76] = INST_ROR_ZPG_X; DBG(0, installed++;)
        instruction[0x09] = INST_ORA_IMM; DBG(0, installed++;)
        instruction[0x19] = INST_ORA_ABS_Y; DBG(0, installed++;)
        instruction[0x29] = INST_AND_IMM; DBG(0, installed++;)
        instruction[0x39] = INST_AND_ABS_Y; DBG(0, installed++;)
        instruction[0x49] = INST_EOR_IMM; DBG(0, installed++;)
        instruction[0x59] = INST_EOR_ABS_Y; DBG(0, installed++;)
        instruction[0x0A] = INST_ASL_A; DBG(0, installed++;)
        instruction[0x2A] = INST_ROL_A; DBG(0, installed++;)
        instruction[0x4A] = INST_LSR_A; DBG(0, installed++;)
        instruction[0x6A] = INST_ROR_A; DBG(0, installed++;)
        instruction[0x0D] = INST_ORA_ABS; DBG(0, installed++;)
        instruction[0x1D] = INST_ORA_ABS_X; DBG(0, installed++;)
        instruction[0x2D] = INST_AND_ABS; DBG(0, installed++;)
        instruction[0x3D] = INST_AND_ABS_X; DBG(0, installed++;)
        instruction[0x4D] = INST_EOR_ABS; DBG(0, installed++;)
        instruction[0x5D] = INST_EOR_ABS_X; DBG(0, installed++;)
        instruction[0x0E] = INST_ASL_ABS; DBG(0, installed++;)
        instruction[0x1E] = INST_ASL_ABS_X; DBG(0, installed++;)
        instruction[0x2E] = INST_ROL_ABS; DBG(0, installed++;)
        instruction[0x3E] = INST_ROL_ABS_X; DBG(0, installed++;)
        instruction[0x4E] = INST_LSR_ABS; DBG(0, installed++;)
        instruction[0x5E] = INST_LSR_ABS_X; DBG(0, installed++;)
        instruction[0x6E] = INST_ROR_ABS; DBG(0, installed++;)
        instruction[0x7E] = INST_ROR_ABS_X; DBG(0, installed++;)
        instruction[0x14] = INST_TRB_ZPG; DBG(0, installed++;)
        instruction[0x1C] = INST_TRB_ABS; DBG(0, installed++;)
        instruction[0x04] = INST_TSB_ZPG; DBG(0, installed++;)
        instruction[0x0C] = INST_TSB_ABS; DBG(0, installed++;)

        // Processor instructions
        instruction[0x00] = INST_BRK; DBG(0, installed++;)
        instruction[0xDB] = INST_STP; DBG(0, installed++;)
        instruction[0xCB] = INST_WAI; DBG(0, installed++;)

        // NOP instruction
        instruction[0xEA] = INST_NOP; DBG(0, installed++;)

        DBG(1, printf("Initialized 65C02 CPU, %d opcodes installed", installed + 37);)
}