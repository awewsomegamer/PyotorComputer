#include "../../include/cpu/instructions/inst_macros.h"

void INST_ADC_IND_X() { ARIT_ADD_SET(PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_ADC_IND_Y() { ARIT_ADD_SET(PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_ADC_ZPG_IND() { ARIT_ADD_SET(PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_SBC_IND_X() { ARIT_SUB_SET(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8)) }
void INST_SBC_IND_Y() { ARIT_SUB_SET(PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_SBC_ZPG_IND() { ARIT_SUB_SET(PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_ADC_ZPG() { ARIT_ADD_SET(PTR(NEXT_BYTE)) }
void INST_ADC_ZPG_X() { ARIT_ADD_SET(PTR(OFF_X(NEXT_BYTE))) }
void INST_SBC_ZPG() { ARIT_SUB_SET(PTR(NEXT_BYTE)) }
void INST_SBC_ZPG_X() { ARIT_SUB_SET(PTR(OFF_X(NEXT_BYTE))) }
void INST_ADC_IMM() { ARIT_ADD_SET(NEXT_BYTE) }
void INST_ADC_ABS_Y() { ARIT_ADD_SET(PTR(OFF_Y(NEXT_WORD))) }
void INST_SBC_IMM() { ARIT_SUB_SET(NEXT_BYTE) }
void INST_SBC_ABS_Y() { ARIT_SUB_SET(PTR(OFF_Y(NEXT_WORD))) }
void INST_ADC_ABS() { ARIT_ADD_SET(PTR(NEXT_WORD)) }
void INST_ADC_ABS_X() { ARIT_ADD_SET(PTR(OFF_X_ABS(NEXT_WORD))) }
void INST_SBC_ABS() { ARIT_SUB_SET(PTR(NEXT_WORD)) }
void INST_SBC_ABS_X() { ARIT_SUB_SET(PTR(OFF_X_ABS(NEXT_WORD))) }

void init_arithmetic_instructions() {
        instruction[0x61] = INST_ADC_IND_X; DBG(0, installed_instructions++;)
        instruction[0x71] = INST_ADC_IND_Y; DBG(0, installed_instructions++;)
        instruction[0x72] = INST_ADC_ZPG_IND; DBG(0, installed_instructions++;)
        instruction[0xE1] = INST_SBC_IND_X; DBG(0, installed_instructions++;)
        instruction[0xF1] = INST_SBC_IND_Y; DBG(0, installed_instructions++;)
        instruction[0xF2] = INST_SBC_ZPG_IND; DBG(0, installed_instructions++;)
        instruction[0x65] = INST_ADC_ZPG; DBG(0, installed_instructions++;)
        instruction[0x75] = INST_ADC_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0xE5] = INST_SBC_ZPG; DBG(0, installed_instructions++;)
        instruction[0xF5] = INST_SBC_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x69] = INST_ADC_IMM; DBG(0, installed_instructions++;)
        instruction[0x79] = INST_ADC_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0xE9] = INST_SBC_IMM; DBG(0, installed_instructions++;)
        instruction[0xF9] = INST_SBC_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0x6D] = INST_ADC_ABS; DBG(0, installed_instructions++;)
        instruction[0x7D] = INST_ADC_ABS_X; DBG(0, installed_instructions++;)
        instruction[0xED] = INST_SBC_ABS; DBG(0, installed_instructions++;)
        instruction[0xFD] = INST_SBC_ABS_X; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed arithmetic instructions");)
}
