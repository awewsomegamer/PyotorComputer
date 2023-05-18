#include "cpu/cpu.h"
#include <cpu/instructions/instruction_macros.h>
#include <stdio.h>

void INST_RTI() {
        // Return from interrupt
        INST_PLP();
        pc = PTR(0x100 + ++register_s) | (PTR(0x100 + ++register_s) << 8);
}

void INST_JSR() {
        mem_byte_write(((pc + 2) >> 8) & 0xFF, 0x100 + (register_s--));
        mem_byte_write((pc + 2) & 0xFF, 0x100 + (register_s--));
        pc = NEXT_WORD;
}

void INST_RTS() {
        // Return from sub-routine
        pc = PTR(0x100 + ++register_s) | (PTR(0x100 + ++register_s) << 8);
}

void INST_JMP_ABS() { pc = NEXT_WORD; }
void INST_JMP_IND() { pc = PTR(CUR_WORD) | (PTR(NEXT_WORD) << 8); }
void INST_JMP_ABS_X() { pc = OFF_X_ABS(NEXT_WORD); }
void INST_BPL_REL() { if (!register_p.N) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BMI_REL() { if (register_p.N) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BVC_REL() { if (!register_p.V) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BVS_REL() { if (register_p.V) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BCC_REL() { if (!register_p.C) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BCS_REL() { if (register_p.C) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BNE_REL() { if (!register_p.Z) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BEQ_REL() { if (register_p.Z) pc += (int8_t)NEXT_BYTE; else pc++; }
void INST_BRA_REL() { pc += (int8_t)NEXT_BYTE; }

void init_control_flow_instructions() {
        instruction[0x40] = INST_RTI; DBG(0, installed_instructions++;)
        instruction[0x20] = INST_JSR; DBG(0, installed_instructions++;)
        instruction[0x60] = INST_RTS; DBG(0, installed_instructions++;)
        instruction[0x4C] = INST_JMP_ABS; DBG(0, installed_instructions++;)
        instruction[0x6C] = INST_JMP_IND; DBG(0, installed_instructions++;)
        instruction[0x10] = INST_BPL_REL; DBG(0, installed_instructions++;)
        instruction[0x30] = INST_BMI_REL; DBG(0, installed_instructions++;)
        instruction[0x50] = INST_BVC_REL; DBG(0, installed_instructions++;)
        instruction[0x70] = INST_BVS_REL; DBG(0, installed_instructions++;)
        instruction[0x90] = INST_BCC_REL; DBG(0, installed_instructions++;)
        instruction[0xB0] = INST_BCS_REL; DBG(0, installed_instructions++;)
        instruction[0xD0] = INST_BNE_REL; DBG(0, installed_instructions++;)
        instruction[0xF0] = INST_BEQ_REL; DBG(0, installed_instructions++;)
        instruction[0x7C] = INST_JMP_ABS_X; DBG(0, installed_instructions++;)
        instruction[0x80] = INST_BRA_REL; DBG(0, installed_instructions++;)

        DBG(1, printf("Installed control flow instructions");)
}
