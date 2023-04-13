#include <cpu/instructions/instruction_macros.h>

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
void INST_STA_ABS_X() { mem_byte_write(register_a, OFF_X_ABS(NEXT_WORD)); }
void INST_STA_ABS_Y() { mem_byte_write(register_a, OFF_Y(NEXT_WORD)); }
void INST_STA_ZPG_IND() { mem_byte_write(register_a, (PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))); }
void INST_STZ_ZPG() { mem_byte_write(0x00, NEXT_BYTE); }
void INST_STZ_ZPG_X() { mem_byte_write(0x00, OFF_X(NEXT_BYTE)); }
void INST_STZ_ABS() { mem_byte_write(0x00, NEXT_WORD); }
void INST_STZ_ABS_X() { mem_byte_write(0x00, OFF_X_ABS(NEXT_WORD)); }

void init_store_instructions() {
        instruction[0x8D] = INST_STA_ABS; DBG(0, installed_instructions++;)
        instruction[0x8E] = INST_STX_ABS; DBG(0, installed_instructions++;)
        instruction[0x8C] = INST_STY_ABS; DBG(0, installed_instructions++;)
        instruction[0x85] = INST_STA_ZPG; DBG(0, installed_instructions++;)
        instruction[0x86] = INST_STX_ZPG; DBG(0, installed_instructions++;)
        instruction[0x84] = INST_STY_ZPG; DBG(0, installed_instructions++;)
        instruction[0x95] = INST_STA_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x96] = INST_STX_ZPG_Y; DBG(0, installed_instructions++;)
        instruction[0x94] = INST_STY_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x81] = INST_STA_IND_X; DBG(0, installed_instructions++;)
        instruction[0x91] = INST_STA_IND_Y; DBG(0, installed_instructions++;)
        instruction[0x9D] = INST_STA_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x99] = INST_STA_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0x92] = INST_STA_ZPG_IND; DBG(0, installed_instructions++;)
        instruction[0x64] = INST_STZ_ZPG; DBG(0, installed_instructions++;)
        instruction[0x74] = INST_STZ_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x9C] = INST_STZ_ABS; DBG(0, installed_instructions++;)
        instruction[0x9E] = INST_STZ_ABS_X; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed store instructions");)
}
