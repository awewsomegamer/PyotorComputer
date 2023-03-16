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

void INST_LDA_ABS() { register_a = PTR(NEXT_BYTE | (NEXT_BYTE << 8)); }
void INST_LDX_ABS() { register_x = PTR(NEXT_BYTE | (NEXT_BYTE << 8)); }
void INST_LDY_ABS() { register_y = PTR(NEXT_BYTE | (NEXT_BYTE << 8)); }

void INST_LDA_ZPG() { register_a = PTR(NEXT_BYTE); }
void INST_LDX_ZPG() { register_x = PTR(NEXT_BYTE); }
void INST_LDY_ZPG() { register_y = PTR(NEXT_BYTE); }

void INST_LDA_ZPG_X() { register_a = PTR(OFF_X(NEXT_BYTE)); }
void INST_LDX_ZPG_Y() { register_x = PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }
void INST_LDY_ZPG_X() { register_y = PTR(OFF_X(NEXT_BYTE)); }

void INST_LDA_IND_X() { register_a = PTR(OFF_X(NEXT_BYTE)); }
void INST_LDA_IND_Y() { register_a = PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }

void INST_LDA_ABS_X() { register_a = PTR(OFF_X(NEXT_BYTE | (NEXT_BYTE << 8))); }
void INST_LDA_ABS_Y() { register_a = PTR(OFF_Y(NEXT_BYTE | (NEXT_BYTE << 8))); }

void INST_LDY_ABS_X() { register_y = PTR(OFF_X(NEXT_BYTE | (NEXT_BYTE << 8))); }
void INST_LDX_ABS_Y() { register_x = PTR(OFF_Y(NEXT_BYTE | (NEXT_BYTE << 8))); }

// ST instructions
void INST_STA_ABS() { mem_byte_write(register_a, NEXT_BYTE | (NEXT_BYTE << 8)); }
void INST_STX_ABS() { mem_byte_write(register_x, NEXT_BYTE | (NEXT_BYTE << 8)); }
void INST_STY_ABS() { mem_byte_write(register_y, NEXT_BYTE | (NEXT_BYTE << 8)); }

void INST_STA_ZPG() { mem_byte_write(register_a, NEXT_BYTE); }
void INST_STX_ZPG() { mem_byte_write(register_x, NEXT_BYTE); }
void INST_STY_ZPG() { mem_byte_write(register_y, NEXT_BYTE); }

void INST_STA_ZPG_X() { mem_byte_write(register_a, OFF_X(NEXT_BYTE)); }
void INST_STX_ZPG_Y() { mem_byte_write(register_x, OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }
void INST_STY_ZPG_X() { mem_byte_write(register_y, OFF_X(NEXT_BYTE)); }

void INST_STA_IND_X() { mem_byte_write(register_a, OFF_X(NEXT_BYTE)); }
void INST_STA_IND_Y() { mem_byte_write(register_a, OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }

void INST_STA_ABS_X() { mem_byte_write(register_a, OFF_X(NEXT_BYTE | (NEXT_BYTE << 8))); }
void INST_STA_ABS_Y() { mem_byte_write(register_a, OFF_Y(NEXT_BYTE | (NEXT_BYTE << 8))); }

void reg_dump_6502() {
        printf("-- 6502 REG DUMP --\n");
        printf("A  : %02X (%d)\n", register_a, register_a);
        printf("X  : %02X (%d)\n", register_x, register_x);
        printf("Y  : %02X (%d)\n", register_y, register_y);
        printf("S  : %02X (%d)\n", register_s, register_s);
        printf("P  : %02X (%d)\n", register_p, register_p);
        printf("PC : %04X (%d)\n", pc, pc);
        printf("-- 6502 REG DUMP END --\n");
}

// Initialize the 6502
void init_6502() {
        DBG(1, "Initializing 6502 CPU")

        // LD instructions
        instruction[0xA9] = INST_LDA_IMM;
        instruction[0xA2] = INST_LDX_IMM;
        instruction[0xA0] = INST_LDY_IMM;

        instruction[0xAD] = INST_LDA_ABS;
        instruction[0xAE] = INST_LDX_ABS;
        instruction[0xAC] = INST_LDY_ABS;

        instruction[0xA5] = INST_LDA_ZPG;
        instruction[0xA6] = INST_LDX_ZPG;
        instruction[0xA4] = INST_LDY_ZPG;

        instruction[0xB5] = INST_LDA_ZPG_X;
        instruction[0xB6] = INST_LDX_ZPG_Y;
        instruction[0xB4] = INST_LDY_ZPG_X;

        instruction[0xA1] = INST_LDA_IND_X;
        instruction[0xB1] = INST_LDA_IND_Y;

        instruction[0xBD] = INST_LDA_ABS_X;
        instruction[0xB9] = INST_LDA_ABS_Y;

        instruction[0xBC] = INST_LDY_ABS_X;
        instruction[0xBE] = INST_LDX_ABS_Y;

        // ST instructions
        instruction[0x8D] = INST_STA_ABS;
        instruction[0x8E] = INST_STX_ABS;
        instruction[0x8C] = INST_STY_ABS;

        instruction[0x85] = INST_STA_ZPG;
        instruction[0x86] = INST_STX_ZPG;
        instruction[0x84] = INST_STY_ZPG;

        instruction[0x95] = INST_STA_ZPG_X;
        instruction[0x96] = INST_STX_ZPG_Y;
        instruction[0x94] = INST_STY_ZPG_X;

        instruction[0x81] = INST_STA_IND_X;
        instruction[0x91] = INST_STA_IND_Y;

        instruction[0x9D] = INST_STA_ABS_X;
        instruction[0x99] = INST_STA_ABS_Y;
        
        DBG(1, "Initialized 6502 CPU")
}