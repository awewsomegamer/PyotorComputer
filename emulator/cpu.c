#include <cpu.h>
#include <ram.h>

// Initial states of registers
uint8_t io_port = 0;
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

void (*instruction[0xFF])();

// Preform one cycle on the 6502
void tick_6502() {
        (*instruction[NEXT_BYTE])();
}

// LD instructions
void INST_LDA_IMM() { register_a = NEXT_BYTE; }
void INST_LDX_IMM() { register_x = NEXT_BYTE; }
void INST_LDY_IMM() { register_y = NEXT_BYTE; }

void INST_LDA_ABS() { register_a = PTR(NEXT_WORD); }
void INST_LDX_ABS() { register_x = PTR(NEXT_WORD); }
void INST_LDY_ABS() { register_y = PTR(NEXT_WORD); }

void INST_LDA_ZPG() { register_a = PTR(NEXT_BYTE); }
void INST_LDX_ZPG() { register_x = PTR(NEXT_BYTE); }
void INST_LDY_ZPG() { register_y = PTR(NEXT_BYTE); }

void INST_LDA_ZPG_X() { register_a = PTR(OFF_X(NEXT_BYTE)); }
void INST_LDX_ZPG_Y() { register_x = PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }
void INST_LDY_ZPG_X() { register_y = PTR(OFF_X(NEXT_BYTE)); }

void INST_LDA_IND_X() { register_a = PTR(OFF_X(NEXT_BYTE)); }
void INST_LDA_IND_Y() { register_a = PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }

void INST_LDA_ABS_X() { register_a = PTR(OFF_X(NEXT_WORD)); }
void INST_LDA_ABS_Y() { register_a = PTR(OFF_Y(NEXT_WORD)); }

void INST_LDY_ABS_X() { register_y = PTR(OFF_X(NEXT_WORD)); }
void INST_LDX_ABS_Y() { register_x = PTR(OFF_Y(NEXT_WORD)); }

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

void INST_STA_IND_X() { mem_byte_write(register_a, OFF_X(NEXT_BYTE)); }
void INST_STA_IND_Y() { mem_byte_write(register_a, OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }

void INST_STA_ABS_X() { mem_byte_write(register_a, OFF_X(NEXT_WORD)); }
void INST_STA_ABS_Y() { mem_byte_write(register_a, OFF_Y(NEXT_WORD)); }

// Stack instructions
void INST_PHP() { mem_byte_write(*(uint8_t *)&register_p, 0x100 + (register_s--)); } // PUSH P
void INST_PLP() { *(uint8_t *)&register_p = mem_byte_read(0x100 + (++register_s)); } // PULL P
void INST_PHA() { mem_byte_write(register_a, 0x100 + (register_s--)); } // PUSH A
void INST_PLA() { register_a = mem_byte_read(0x100 + (++register_s)); } // PULL A

// P register (Flag) instructions
void INST_SED() { register_p.D = 1; }
void INST_CLD() { register_p.D = 0; }
void INST_CLV() { register_p.V = 0; }
void INST_SEI() { register_p.I = 1; }
void INST_CLI() { register_p.I = 0; }
void INST_SEC() { register_p.C = 1; }
void INST_CLC() { register_p.C = 0; }

// Increment / Decrement instructions
void INST_INX() { register_x++; }
void INST_DEX() { register_x--; }
void INST_INY() { register_y++; }
void INST_DEY() { register_y--; }
void INST_INC_ABS() { uint16_t addr = NEXT_WORD; mem_byte_write(mem_byte_read(addr) + 1, addr); }
void INST_DEC_ABS() { uint16_t addr = NEXT_WORD; mem_byte_write(mem_byte_read(addr) - 1, addr); }
void INST_DEC_ZPG() { uint16_t addr = NEXT_BYTE; mem_byte_write(mem_byte_read(addr) - 1, addr); }
void INST_DEC_ZPG_X() { uint16_t addr = OFF_X(NEXT_BYTE); mem_byte_write(mem_byte_read(addr) - 1, addr); }
void INST_INC_ZPG() { uint16_t addr = NEXT_BYTE; mem_byte_write(mem_byte_read(addr) + 1, addr); }
void INST_INC_ZPG_X() { uint16_t addr = OFF_X(NEXT_BYTE); mem_byte_write(mem_byte_read(addr) + 1, addr); }
void INST_DEC_ABS_X() { uint16_t addr = OFF_X(NEXT_WORD); mem_byte_write(mem_byte_read(addr) - 1, addr); }
void INST_INC_ABS_X() { uint16_t addr = OFF_X(NEXT_WORD); mem_byte_write(mem_byte_read(addr) + 1, addr); }

// Transfer instructions
void INST_TYA() { register_a = register_y; }
void INST_TAY() { register_y = register_a; }
void INST_TXA() { register_a = register_x; }
void INST_TXS() { register_s = register_x; }
void INST_TAX() { register_x = register_a; }
void INST_TSX() { register_x = register_s; }

// Control flow instructions
void INST_BRK() {
        mem_byte_write(((pc + 2) >> 8) & 0xFF, 0x100 + (register_s--));
        mem_byte_write((pc + 2) & 0xFF, 0x100 + (register_s--));
        INST_PHP();
        register_p.B = 1;
        register_p.I = 1;
}

void INST_RTI() {
        // Return from interrupt
        INST_PLP();
        pc = mem_byte_read(0x100 + ++register_s) | (mem_byte_read(0x100 + ++register_s) << 8);
} 

void INST_JSR() {
        mem_byte_write(((pc + 2) >> 8) & 0xFF, 0x100 + (register_s--));
        mem_byte_write((pc + 2) & 0xFF, 0x100 + (register_s--));
        pc = NEXT_WORD;
}

void INST_RTS() {
        // Return from sub-routine
        pc = mem_byte_read(0x100 + ++register_s) | (mem_byte_read(0x100 + ++register_s) << 8);
}

void INST_JMP_ABS() { pc = NEXT_WORD; }
void INST_JMP_IND() { pc = PTR(NEXT_WORD) | (PTR(NEXT_WORD) << 8); }

// Logical arithmetic instructions
void INST_ASL_A() { } // Shift the A register left (N: original bit in bit 6, Z: if the result is == 0, otherwise it is resets Z and stores the original bit 7 in the carry)
void INST_ROL_A() { } // Rotate the A register left 1, old carry overflows to bit 0, old bit 7 overflows to carry
void INST_LSR_A() { } // Logical Shift Right shift right by 1 bit 0 is shifted into bit 7 and original bit 0 overflows to carry
void INST_ROR_A() { } // Rotate the A register right 1, old cary overflows to bit 7, old bit 0 overflows to carry

// Comparison instructions
void INST_CPX_IMM() { CMP_SET(register_x, NEXT_BYTE) }
void INST_CPY_IMM() { CMP_SET(register_y, NEXT_BYTE) }
void INST_CPX_ZPG() { uint16_t addr = NEXT_BYTE; CMP_SET(register_x, mem_byte_read(addr)) }
void INST_CPY_ZPG() { uint16_t addr = NEXT_BYTE; CMP_SET(register_y, mem_byte_read(addr)) }
void INST_CMP_ZPG() { uint16_t addr = NEXT_BYTE; CMP_SET(register_a, mem_byte_read(addr)) }
void INST_CMP_ZPG_X() { uint16_t addr = OFF_X(NEXT_BYTE); CMP_SET(register_a, mem_byte_read(addr)) }
void INST_CMP_IMM() { CMP_SET(register_a, NEXT_BYTE) }
void INST_CMP_ABS_Y() { uint16_t addr = OFF_Y(NEXT_WORD); CMP_SET(register_a, mem_byte_read(addr)) }
void INST_CPX_ABS() { uint16_t addr = NEXT_WORD; CMP_SET(register_x, mem_byte_read(addr)) }
void INST_CPY_ABS() { uint16_t addr = NEXT_WORD; CMP_SET(register_y, mem_byte_read(addr)) }
void INST_CMP_ABS() { uint16_t addr = NEXT_WORD; CMP_SET(register_a, mem_byte_read(addr)) }
void INST_CMP_ABS_X() { uint16_t addr = OFF_X(NEXT_WORD); CMP_SET(register_a, mem_byte_read(addr)) }

void INST_NOP() { ; }

void reg_dump_6502() {
        printf("-- 6502 REG DUMP --\n");
        printf("A  : %02X (%d)\n", register_a, register_a);
        printf("X  : %02X (%d)\n", register_x, register_x);
        printf("Y  : %02X (%d)\n", register_y, register_y);
        printf("S  : %02X (%d)\n", register_s, register_s);

        printf("P  : %02X (", register_p, register_p);
        if (register_p.N) printf("N");
        if (register_p.V) printf(" | V");
        if (register_p.unused) printf(" | Unused");
        if (register_p.B) printf(" | B");
        if (register_p.D) printf(" | D");
        if (register_p.I) printf(" | I");
        if (register_p.Z) printf(" | Z");
        if (register_p.C) printf(" | C");
        printf(")\n");


        printf("PC : %04X (%d)\n", pc, pc);
        printf("-- 6502 REG DUMP END --\n");
}

// Initialize the 6502
void init_6502() {
        DBG(1, printf("Initializing 6502 CPU");)

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

        // Stack instructions
        instruction[0x08] = INST_PHP; DBG(0, installed++;)
        instruction[0x28] = INST_PLP; DBG(0, installed++;)
        instruction[0x48] = INST_PHA; DBG(0, installed++;)
        instruction[0x68] = INST_PLA; DBG(0, installed++;)

        // P register (Flag) instructions
        instruction[0xF8] = INST_SED; DBG(0, installed++;)
        instruction[0xD8] = INST_CLD; DBG(0, installed++;)
        instruction[0xB8] = INST_CLV; DBG(0, installed++;)
        instruction[0x78] = INST_SEI; DBG(0, installed++;)
        instruction[0x58] = INST_CLI; DBG(0, installed++;)
        instruction[0x38] = INST_SEC; DBG(0, installed++;)
        instruction[0x18] = INST_CLC; DBG(0, installed++;)

        // Increment / Decrement instructions
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
        // instruction[0x00] = INST_BRK;
        instruction[0x40] = INST_RTI; DBG(0, installed++;)
        instruction[0x20] = INST_JSR; DBG(0, installed++;)
        instruction[0x60] = INST_RTS; DBG(0, installed++;)
        instruction[0x4C] = INST_JMP_ABS; DBG(0, installed++;)
        instruction[0x6C] = INST_JMP_IND; DBG(0, installed++;)

        // Compare instructions
        instruction[0xE0] = INST_CPX_IMM; DBG(0, installed++;)
        instruction[0xC0] = INST_CPY_IMM; DBG(0, installed++;)
        instruction[0xE4] = INST_CPX_ZPG; DBG(0, installed++;)
        instruction[0xC4] = INST_CPY_ZPG; DBG(0, installed++;)
        instruction[0xC5] = INST_CMP_ZPG; DBG(0, installed++;)
        instruction[0xD5] = INST_CMP_ZPG_X; DBG(0, installed++;)
        instruction[0xC9] = INST_CMP_IMM; DBG(0, installed++;)
        instruction[0xD9] = INST_CMP_ABS_Y; DBG(0, installed++;)
        instruction[0xDC] = INST_CPX_ABS; DBG(0, installed++;)
        instruction[0xCC] = INST_CPY_ABS; DBG(0, installed++;)
        instruction[0xCD] = INST_CMP_ABS; DBG(0, installed++;)
        instruction[0xDD] = INST_CMP_ABS_X; DBG(0, installed++;)

        // NOP instruction
        instruction[0xEA] = INST_NOP;

        DBG(1, printf("Initialized 6502 CPU, %d instructions installed.", installed);)
}