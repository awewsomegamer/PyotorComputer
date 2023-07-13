#include <cpu/instructions/inst_macros.h>

void INST_TYA() { register_a = register_y; SET_NZ(register_a) }
void INST_TAY() { register_y = register_a; SET_NZ(register_y) }
void INST_TXA() { register_a = register_x; SET_NZ(register_a) }
void INST_TXS() { register_s = register_x; }
void INST_TAX() { register_x = register_a; SET_NZ(register_x) }
void INST_TSX() { register_x = register_s; SET_NZ(register_x) }

void init_transfer_instructions() {
        instruction[0x98] = INST_TYA; DBG(0, installed_instructions++;)
        instruction[0xA8] = INST_TAY; DBG(0, installed_instructions++;)
        instruction[0x8A] = INST_TXA; DBG(0, installed_instructions++;)
        instruction[0x9A] = INST_TXS; DBG(0, installed_instructions++;)
        instruction[0xAA] = INST_TAX; DBG(0, installed_instructions++;)
        instruction[0xBA] = INST_TSX; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed transfer instructions");)
}