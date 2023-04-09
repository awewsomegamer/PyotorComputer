#include <cpu/instructions/instruction_macros.h>

void INST_PHP() { mem_byte_write(*(uint8_t *)&register_p, 0x100 + (register_s--)); } // PUSH P
void INST_PLP() { *(uint8_t *)&register_p = PTR(0x100 + (++register_s)); } // PULL P
void INST_PHA() { mem_byte_write(register_a, 0x100 + (register_s--)); } // PUSH A
void INST_PLA() { register_a = PTR(0x100 + (++register_s)); SET_NZ(register_a)} // PULL A
void INST_PHX() { mem_byte_write(register_x, 0x100 + (register_s--)); } // PUSH X
void INST_PLX() { register_x = PTR(0x100 + (++register_s)); SET_NZ(register_x)} // PULL X
void INST_PHY() { mem_byte_write(register_y, 0x100 + (register_s--)); } // PUSH Y
void INST_PLY() { register_y = PTR(0x100 + (++register_s)); SET_NZ(register_y)} // PULL Y

void init_stack_instructions() {
        instruction[0x08] = INST_PHP; DBG(0, installed_instructions++;)
        instruction[0x28] = INST_PLP; DBG(0, installed_instructions++;)
        instruction[0x48] = INST_PHA; DBG(0, installed_instructions++;)
        instruction[0x68] = INST_PLA; DBG(0, installed_instructions++;)
        instruction[0xDA] = INST_PHX; DBG(0, installed_instructions++;)
        instruction[0xFA] = INST_PLX; DBG(0, installed_instructions++;)
        instruction[0x5A] = INST_PHY; DBG(0, installed_instructions++;)
        instruction[0x7A] = INST_PLY; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed stack instructions");)
}