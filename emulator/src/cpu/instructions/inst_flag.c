#include "../../include/cpu/instructions/inst_macros.h"
#include <stdio.h>

void INST_SED() { register_p->D = 1; }
void INST_CLD() { register_p->D = 0; }
void INST_CLV() { register_p->V = 0; }
void INST_SEI() { register_p->I = 1; }
void INST_CLI() { register_p->I = 0; }
void INST_SEC() { register_p->C = 1; }
void INST_CLC() { register_p->C = 0; }

void init_flag_instructions() {
        instruction[0xF8] = INST_SED; DBG(0, installed_instructions++;)
        instruction[0xD8] = INST_CLD; DBG(0, installed_instructions++;)
        instruction[0xB8] = INST_CLV; DBG(0, installed_instructions++;)
        instruction[0x78] = INST_SEI; DBG(0, installed_instructions++;)
        instruction[0x58] = INST_CLI; DBG(0, installed_instructions++;)
        instruction[0x38] = INST_SEC; DBG(0, installed_instructions++;)
        instruction[0x18] = INST_CLC; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed flag instructions");)
}