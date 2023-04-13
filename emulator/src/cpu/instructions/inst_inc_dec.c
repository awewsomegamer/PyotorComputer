#include <cpu/instructions/instruction_macros.h>

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
void INST_DEC_ABS_X() { uint16_t addr = OFF_X_ABS(NEXT_WORD); mem_byte_write(PTR(addr) - 1, addr); SET_NZ(PTR(addr)) }
void INST_INC_ABS_X() { uint16_t addr = OFF_X_ABS(NEXT_WORD); mem_byte_write(PTR(addr) + 1, addr); SET_NZ(PTR(addr)) }

void init_inc_dec_instructions() {
        instruction[0x1A] = INST_INA; DBG(0, installed_instructions++;)
        instruction[0x3A] = INST_DEA; DBG(0, installed_instructions++;)
        instruction[0xE8] = INST_INX; DBG(0, installed_instructions++;)
        instruction[0xCA] = INST_DEX; DBG(0, installed_instructions++;)
        instruction[0xC8] = INST_INY; DBG(0, installed_instructions++;)
        instruction[0x88] = INST_DEY; DBG(0, installed_instructions++;)
        instruction[0xEE] = INST_INC_ABS; DBG(0, installed_instructions++;)
        instruction[0xCE] = INST_DEC_ABS; DBG(0, installed_instructions++;)
        instruction[0xC6] = INST_DEC_ZPG; DBG(0, installed_instructions++;)
        instruction[0xD6] = INST_DEC_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0xE6] = INST_INC_ZPG; DBG(0, installed_instructions++;)
        instruction[0xF6] = INST_INC_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0xDE] = INST_DEC_ABS_X; DBG(0, installed_instructions++;)
        instruction[0xFE] = INST_INC_ABS_X; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed increment / decrement instructions");)
}
