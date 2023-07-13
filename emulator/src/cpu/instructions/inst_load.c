#include <cpu/instructions/inst_macros.h>

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
void INST_LDA_ABS_X() { register_a = PTR(OFF_X_ABS(NEXT_WORD)); SET_NZ(register_x) }
void INST_LDA_ABS_Y() { register_a = PTR(OFF_Y(NEXT_WORD)); SET_NZ(register_y) }
void INST_LDY_ABS_X() { register_y = PTR(OFF_X_ABS(NEXT_WORD)); SET_NZ(register_x) }
void INST_LDX_ABS_Y() { register_x = PTR(OFF_Y(NEXT_WORD)); SET_NZ(register_y) }
void INST_LDA_ZPG_IND() { register_a = PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)); SET_NZ(register_a) }

void init_load_instructions() {
        instruction[0xA9] = INST_LDA_IMM; DBG(0, installed_instructions++;)
        instruction[0xA2] = INST_LDX_IMM; DBG(0, installed_instructions++;)
        instruction[0xA0] = INST_LDY_IMM; DBG(0, installed_instructions++;)
        instruction[0xAD] = INST_LDA_ABS; DBG(0, installed_instructions++;)
        instruction[0xAE] = INST_LDX_ABS; DBG(0, installed_instructions++;)
        instruction[0xAC] = INST_LDY_ABS; DBG(0, installed_instructions++;)
        instruction[0xA5] = INST_LDA_ZPG; DBG(0, installed_instructions++;)
        instruction[0xA6] = INST_LDX_ZPG; DBG(0, installed_instructions++;)
        instruction[0xA4] = INST_LDY_ZPG; DBG(0, installed_instructions++;)
        instruction[0xB5] = INST_LDA_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0xB6] = INST_LDX_ZPG_Y; DBG(0, installed_instructions++;)
        instruction[0xB4] = INST_LDY_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0xA1] = INST_LDA_IND_X; DBG(0, installed_instructions++;)
        instruction[0xB1] = INST_LDA_IND_Y; DBG(0, installed_instructions++;)
        instruction[0xBD] = INST_LDA_ABS_X; DBG(0, installed_instructions++;)
        instruction[0xB9] = INST_LDA_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0xBC] = INST_LDY_ABS_X; DBG(0, installed_instructions++;)
        instruction[0xBE] = INST_LDX_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0xB2] = INST_LDA_ZPG_IND; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed load instructions");)
}
