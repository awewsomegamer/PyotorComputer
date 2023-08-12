#include "../../include/cpu/cpu.h"
#include "../../include/cpu/instructions/inst_macros.h"

uint8_t ARIT_LEFT_SHIFT(uint8_t what) {
        uint8_t result = (what << 1);
        SET_NZ(result)
        register_p->C = (what << 7) & 1;
        return result;
}

uint8_t LOGICAL_RIGHT_SHIFT(uint8_t what) {
        uint8_t result = (what >> 1);
        SET_NZ(result)
        register_p->C = (what & 1);
        return result;
}

uint8_t ROTATE_LEFT_SHIFT(uint8_t what) {
        uint8_t result = (what << 1) | register_p->C;
        SET_NZ(result)
        register_p->C = (what >> 7) & 1;
        return result;
}

uint8_t ROTATE_RIGHT_SHIFT(uint8_t what) {
        uint8_t result = (what >> 1) | (register_p->C << 7);
        SET_NZ(result)
        register_p->C = (what & 1);
        return result;
}

void INST_ASL_A() { *register_a = ARIT_LEFT_SHIFT(*register_a); } // Shift the A register left (N: original bit in bit 6, Z: if the result is == 0, otherwise it is resets Z and stores the original bit 7 in the carry)
void INST_ROL_A() { *register_a = ROTATE_LEFT_SHIFT(*register_a); } // Rotate the A register left 1, old carry overflows to bit 0, old bit 7 overflows to carry
void INST_LSR_A() { *register_a = LOGICAL_RIGHT_SHIFT(*register_a); register_p->N = 0;} // Logical Shift Right shift right by 1 bit 0 is shifted into bit 7 and original bit 0 overflows to carry
void INST_ROR_A() { *register_a = ROTATE_RIGHT_SHIFT(*register_a); } // Rotate the A register right 1, old cary overflows to bit 7, old bit 0 overflows to carry

void INST_ORA_IND_X() { ARIT_OR_SET(*register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_ORA_IND_Y() { ARIT_OR_SET(*register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_ORA_ZPG_IND() { ARIT_OR_SET(*register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }
void INST_AND_IND_X() { ARIT_AND_SET(*register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_AND_IND_Y() { ARIT_AND_SET(*register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_EOR_IND_X() { ARIT_EOR_SET(*register_a, PTR(PTR(OFF_X(CUR_BYTE)) | (PTR(OFF_X(NEXT_BYTE + 1)) << 8))) }
void INST_EOR_IND_Y() { ARIT_EOR_SET(*register_a, PTR(OFF_Y(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8)))) }
void INST_EOR_ZPG_IND() { ARIT_EOR_SET(*register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) }

void INST_TRB_ZPG() {
        uint8_t result = PTR(CUR_BYTE) & (*register_a ^ 0xFF);
        register_p->Z = (PTR(CUR_BYTE) == 0);
        mem_byte_write(NEXT_BYTE, result);
}

void INST_TRB_ABS() {
        uint8_t result = PTR(CUR_WORD) & (*register_a ^ 0xFF);
        register_p->Z = (PTR(CUR_WORD) == 0);
        mem_byte_write(NEXT_WORD, result);
}

void INST_TSB_ZPG() {
        uint8_t result = PTR(CUR_BYTE) | *register_a;
        register_p->Z = (PTR(CUR_BYTE) == 0);
        mem_byte_write(NEXT_BYTE, result);
}

void INST_TSB_ABS() {
        uint8_t result = PTR(CUR_WORD) | *register_a;
        register_p->Z = (PTR(CUR_WORD) == 0);
        mem_byte_write(NEXT_WORD, result);
}

void INST_ORA_ZPG() { ARIT_OR_SET(*register_a, PTR(NEXT_BYTE)) }
void INST_ORA_ZPG_X() { ARIT_OR_SET(*register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_AND_ZPG() { ARIT_AND_SET(*register_a, PTR(NEXT_BYTE)) }
void INST_AND_ZPG_X() { ARIT_AND_SET(*register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_AND_ZPG_IND() { ARIT_AND_SET(*register_a, PTR(PTR(CUR_BYTE) | (PTR(NEXT_BYTE + 1) << 8))) } 
void INST_EOR_ZPG() { ARIT_EOR_SET(*register_a, PTR(NEXT_BYTE)) }
void INST_EOR_ZPG_X() { ARIT_EOR_SET(*register_a, PTR(OFF_X(NEXT_BYTE))) }
void INST_ASL_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ASL_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ROL_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(ROTATE_RIGHT_SHIFT(PTR(address)), address); }
void INST_ROL_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(ROTATE_RIGHT_SHIFT(PTR(address)), address); }
void INST_LSR_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(LOGICAL_RIGHT_SHIFT(PTR(address)), address); register_p->N = 0; }
void INST_LSR_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(LOGICAL_RIGHT_SHIFT(PTR(address)), address); register_p->N = 0; }
void INST_ROR_ZPG() { uint16_t address = NEXT_BYTE; mem_byte_write(ROTATE_RIGHT_SHIFT(PTR(address)), address); }
void INST_ROR_ZPG_X() { uint16_t address = OFF_X(NEXT_BYTE); mem_byte_write(ROTATE_RIGHT_SHIFT(PTR(address)), address); }
void INST_ORA_IMM() { ARIT_OR_SET(*register_a, NEXT_BYTE) }
void INST_ORA_ABS_Y() { ARIT_OR_SET(*register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_AND_IMM() { ARIT_AND_SET(*register_a, NEXT_BYTE) }
void INST_AND_ABS_Y() { ARIT_AND_SET(*register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_EOR_IMM() { ARIT_EOR_SET(*register_a, NEXT_BYTE) }
void INST_EOR_ABS_Y() { ARIT_EOR_SET(*register_a, PTR(OFF_Y(NEXT_WORD))) }
void INST_ORA_ABS() { ARIT_OR_SET(*register_a, PTR(NEXT_WORD)) }
void INST_ORA_ABS_X() { ARIT_OR_SET(*register_a, PTR(OFF_X_ABS(NEXT_WORD))) }
void INST_AND_ABS() { ARIT_AND_SET(*register_a, PTR(NEXT_WORD)) }
void INST_AND_ABS_X() { ARIT_AND_SET(*register_a, PTR(OFF_X_ABS(NEXT_WORD))) }
void INST_EOR_ABS() { ARIT_EOR_SET(*register_a, PTR(NEXT_WORD)) }
void INST_EOR_ABS_X() { ARIT_EOR_SET(*register_a, PTR(OFF_X_ABS(NEXT_WORD))) }
void INST_ASL_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ASL_ABS_X() { uint16_t address = OFF_X_ABS(NEXT_WORD); mem_byte_write(ARIT_LEFT_SHIFT(PTR(address)), address); }
void INST_ROL_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(ROTATE_LEFT_SHIFT(PTR(address)), address); }
void INST_ROL_ABS_X() { uint16_t address = OFF_X_ABS(NEXT_WORD); mem_byte_write(ROTATE_LEFT_SHIFT(PTR(address)), address); }
void INST_LSR_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(LOGICAL_RIGHT_SHIFT(PTR(address)), address); register_p->N = 0; }
void INST_LSR_ABS_X() { uint16_t address = OFF_X_ABS(NEXT_WORD); mem_byte_write(LOGICAL_RIGHT_SHIFT(PTR(address)), address); register_p->N = 0; }
void INST_ROR_ABS() { uint16_t address = NEXT_WORD; mem_byte_write(ROTATE_RIGHT_SHIFT(PTR(address)), address); }
void INST_ROR_ABS_X() { uint16_t address = OFF_X_ABS(NEXT_WORD); mem_byte_write(ROTATE_RIGHT_SHIFT(PTR(address)), address); }

void init_logical_instructions() {
        instruction[0x01] = INST_ORA_IND_X; DBG(0, installed_instructions++;)
        instruction[0x11] = INST_ORA_IND_Y; DBG(0, installed_instructions++;)
        instruction[0x12] = INST_ORA_ZPG_IND; DBG(0, installed_instructions++;)
        instruction[0x21] = INST_AND_IND_X; DBG(0, installed_instructions++;)
        instruction[0x31] = INST_AND_IND_Y; DBG(0, installed_instructions++;)
        instruction[0x32] = INST_AND_ZPG_IND; DBG(0, installed_instructions++;)
        instruction[0x41] = INST_EOR_IND_X; DBG(0, installed_instructions++;)
        instruction[0x51] = INST_EOR_IND_Y; DBG(0, installed_instructions++;)
        instruction[0x52] = INST_EOR_ZPG_IND; DBG(0, installed_instructions++;)
        instruction[0x05] = INST_ORA_ZPG; DBG(0, installed_instructions++;)
        instruction[0x15] = INST_ORA_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x25] = INST_AND_ZPG; DBG(0, installed_instructions++;)
        instruction[0x35] = INST_AND_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x45] = INST_EOR_ZPG; DBG(0, installed_instructions++;)
        instruction[0x55] = INST_EOR_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x06] = INST_ASL_ZPG; DBG(0, installed_instructions++;)
        instruction[0x16] = INST_ASL_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x26] = INST_ROL_ZPG; DBG(0, installed_instructions++;)
        instruction[0x36] = INST_ROL_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x46] = INST_LSR_ZPG; DBG(0, installed_instructions++;)
        instruction[0x56] = INST_LSR_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x66] = INST_ROR_ZPG; DBG(0, installed_instructions++;)
        instruction[0x76] = INST_ROR_ZPG_X; DBG(0, installed_instructions++;)
        instruction[0x09] = INST_ORA_IMM; DBG(0, installed_instructions++;)
        instruction[0x19] = INST_ORA_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0x29] = INST_AND_IMM; DBG(0, installed_instructions++;)
        instruction[0x39] = INST_AND_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0x49] = INST_EOR_IMM; DBG(0, installed_instructions++;)
        instruction[0x59] = INST_EOR_ABS_Y; DBG(0, installed_instructions++;)
        instruction[0x0A] = INST_ASL_A; DBG(0, installed_instructions++;)
        instruction[0x2A] = INST_ROL_A; DBG(0, installed_instructions++;)
        instruction[0x4A] = INST_LSR_A; DBG(0, installed_instructions++;)
        instruction[0x6A] = INST_ROR_A; DBG(0, installed_instructions++;)
        instruction[0x0D] = INST_ORA_ABS; DBG(0, installed_instructions++;)
        instruction[0x1D] = INST_ORA_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x2D] = INST_AND_ABS; DBG(0, installed_instructions++;)
        instruction[0x3D] = INST_AND_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x4D] = INST_EOR_ABS; DBG(0, installed_instructions++;)
        instruction[0x5D] = INST_EOR_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x0E] = INST_ASL_ABS; DBG(0, installed_instructions++;)
        instruction[0x1E] = INST_ASL_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x2E] = INST_ROL_ABS; DBG(0, installed_instructions++;)
        instruction[0x3E] = INST_ROL_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x4E] = INST_LSR_ABS; DBG(0, installed_instructions++;)
        instruction[0x5E] = INST_LSR_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x6E] = INST_ROR_ABS; DBG(0, installed_instructions++;)
        instruction[0x7E] = INST_ROR_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x14] = INST_TRB_ZPG; DBG(0, installed_instructions++;)
        instruction[0x1C] = INST_TRB_ABS; DBG(0, installed_instructions++;)
        instruction[0x04] = INST_TSB_ZPG; DBG(0, installed_instructions++;)
        instruction[0x0C] = INST_TSB_ABS; DBG(0, installed_instructions++;)

        DBG(1, printf("Installing logical instructions");)
}
