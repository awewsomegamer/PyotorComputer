#include <cpu/instructions/instruction_macros.h>

void INST_CPX_IMM() { CMP_SET(register_x, NEXT_BYTE) }
void INST_CPY_IMM() { CMP_SET(register_y, NEXT_BYTE) }
void INST_CMP_IND_X() { CMP_SET(register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_CMP_IND_Y() { CMP_SET(register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_CPX_ZPG() { CMP_SET(register_x, PTR(NEXT_BYTE)) }
void INST_CPY_ZPG() { CMP_SET(register_y, PTR(NEXT_BYTE)) }
void INST_CMP_ZPG() { CMP_SET(register_a, PTR(NEXT_BYTE)) }
void INST_CMP_ZPG_X() { CMP_SET(register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_CMP_IMM() { CMP_SET(register_a, NEXT_BYTE) }
void INST_CMP_ABS_Y() {CMP_SET(register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_CPX_ABS() { CMP_SET(register_x, PTR(NEXT_WORD)) }
void INST_CPY_ABS() { CMP_SET(register_y, PTR(NEXT_WORD)) }
void INST_CMP_ABS() { CMP_SET(register_a, PTR(NEXT_WORD)) }
void INST_CMP_ABS_X() { CMP_SET(register_a, PTR(OFF_X_ABS(NEXT_WORD))) }
void INST_CMP_ZPG_IND() { CMP_SET(register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }

void init_comparison_instructions() {
        instruction[0xE0] = INST_CPX_IMM; DBG(0, installed_instructions++;)
        instruction[0xC0] = INST_CPY_IMM; DBG(0, installed_instructions++;)
        instruction[0xC1] = INST_CMP_IND_X; DBG(0, installed_instructions++;)
        instruction[0xE4] = INST_CPX_ZPG; DBG(0, installed_instructions++;)
        instruction[0xC4] = INST_CPY_ZPG; DBG(0, installed_instructions++;)
        instruction[0xC5] = INST_CMP_ZPG; DBG(0, installed_instructions++;)
        instruction[0xD1] = INST_CMP_IND_Y; DBG(0, installed_instructions++;)
        instruction[0xD5] = INST_CMP_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0xC9] = INST_CMP_IMM; DBG(0, installed_instructions++;)
        instruction[0xD9] = INST_CMP_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0xEC] = INST_CPX_ABS; DBG(0, installed_instructions++;)
        instruction[0xCC] = INST_CPY_ABS; DBG(0, installed_instructions++;)
        instruction[0xCD] = INST_CMP_ABS; DBG(0, installed_instructions++;)
        instruction[0xDD] = INST_CMP_ABS_X; DBG(0, installed_instructions++;)
        instruction[0xD2] = INST_CMP_ZPG_IND; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed comparison instructions");)
}
