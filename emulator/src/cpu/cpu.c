#include "../include/global.h"
#include "../include/cpu/cpu.h"
#include "../include/cpu/instructions/inst_macros.h"
#include "../include/ram.h"
#include <stdint.h>
#include <stdio.h>

// ** TODO: Implement BCD mode

// Initial states of registers
uint8_t *register_a = NULL;
uint8_t *register_x = NULL;
uint8_t *register_y = NULL;
uint8_t *register_s = NULL;
struct reg_flags *register_p = NULL;
uint16_t *pc = NULL;
uint16_t *cur_inst = NULL;

// Pins
uint8_t *pins = NULL; // I N R 0 0 0 0 0

uint8_t *emulator_flags = NULL; // W S 0 0 0 0 0 0

// Clock
uint64_t cycle_count = 0;

// Tables
void (*instruction[0xFF])();
int installed_instructions = 0;

//                       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
uint8_t cycles[256] = {  7, 6, 0, 0, 5, 3, 5, 0, 0, 2, 2, 0, 6, 4, 6, 0,   // 0
                         2, 5, 5, 0, 5, 4, 6, 0, 2, 4, 2, 0, 6, 4, 7, 0,   // 1
                         6, 6, 0, 0, 3, 3, 5, 0, 0, 2, 2, 0, 4, 4, 6, 0,   // 2
                         2, 5, 5, 0, 4, 4, 6, 0, 2, 4, 2, 0, 4, 4, 7, 0,   // 3
                         6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,   // 4
                         2, 5, 5, 0, 0, 4, 6, 0, 2, 4, 3, 0, 0, 4, 7, 0,   // 5
                         6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,   // 6
                         2, 5, 5, 0, 4, 4, 6, 0, 2, 4, 4, 0, 6, 4, 7, 0,   // 7
                         3, 6, 0, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,   // 8
                         2, 6, 5, 0, 4, 4, 4, 0, 2, 5, 2, 0, 4, 4, 5, 0,   // 9
                         2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,   // A
                         2, 5, 5, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,   // B
                         2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,   // C
                         2, 5, 5, 0, 0, 4, 6, 0, 2, 4, 3, 0, 0, 4, 7, 0,   // D
                         2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,   // E
                         2, 5, 5, 0, 0, 4, 6, 0, 2, 4, 4, 0, 0, 4, 7, 0 }; // F

// Run-time instructions
void INST_BBS(int index) {
        uint8_t addr = NEXT_BYTE;
        uint8_t off = NEXT_BYTE;

        cycle_count += 5 + (((*pc + (int8_t)off) / PAGE_SIZE == *pc / PAGE_SIZE) ? 1 : 2);

        if (((PTR(addr) >> index) & 1) == 1)
                pc += (int8_t)off;
}

void INST_BBR(int index) {
        uint8_t addr = NEXT_BYTE;
        uint8_t off = NEXT_BYTE;

        cycle_count += 5 + (((*pc + (int8_t)off) / PAGE_SIZE == *pc / PAGE_SIZE) ? 1 : 2);

        if (((PTR(addr) >> index) & 1) == 0)
                pc += (int8_t)off;
}

void INST_SMB(int index) { mem_byte_write(PTR(CUR_BYTE) | (1 << index), NEXT_BYTE); cycle_count += 5; }
void INST_RMB(int index) { mem_byte_write(PTR(CUR_BYTE) & ~(1 << index), NEXT_BYTE); cycle_count += 5; }

void INST_BIT(uint8_t opcode) { 
        uint8_t value = 0;
        switch (PTR(*pc - 1)) {
        case 0x24: // ZPG
                value = PTR(NEXT_BYTE);
                break;
        case 0x2C: // ABS
                value = PTR(NEXT_BYTE);
                break;
        case 0x89: // IMM
                value = NEXT_BYTE;
                break;
        case 0x34: // ZP,X
                value = PTR(OFF_X(NEXT_BYTE));
                break;
        case 0x3C: // ABS,X
                value = PTR(OFF_Y(NEXT_WORD));
                break;
        }

        uint8_t result = value & *register_a;
        register_p->Z = (result == 0);

        if (opcode != 0x89) { // Apparently IMM does not set these flags?
                register_p->N = (result >> 7) & 1;
                register_p->V = (result >> 6) & 1;
        }
}

void call_interrupt() {
        mem_byte_write(((*pc) >> 8) & 0xFF, 0x100 + ((*register_s)--)); // High
        mem_byte_write((*pc) & 0xFF, 0x100 + ((*register_s)--)); // Low

        *emulator_flags &= ~(1 << 7); // Unset waiting flag

        if (!((*pins >> 6) & 1)) {
                // NMI
                *pc = PTR(0xFFFA) | (PTR(0xFFFB) << 8);
                *pins |= 1 << 6; // Not the correct way to do it, but desired effect is achieved
        } else if (!((*pins >> 7) & 1) && !register_p->I) {
                // IRQ
                *pc = PTR(0xFFFE) | (PTR(0xFFFF) << 8);
        } else if (!((*pins >> 5) & 1)) {
                // Reset
                *emulator_flags &= ~(1 << 6); // Unset stopped flag

                *pc = PTR(0xFFFC) | (PTR(0xFFFD) << 8);
                *pins |= 1 << 5; // Not the correct way to do it, but desired effect is achieved
        }

        INST_PHP(); // Push flags
        register_p->I = 1; // Disable interrupts
}

// Preform one cycle on the 6502
void tick_65C02() {
        if ((!((*pins >> 7) & 1) && !register_p->I) || !((*pins >> 6) & 1) || !((*pins >> 5) & 1)) // NMI, RES, or IRQ pins went low, check if IRQs are unmasked
                call_interrupt();


        if (((*emulator_flags >> 7) & 1) || ((*emulator_flags >> 6) & 1)) {
                // We are waiting
                cycle_count++; // Should this be done this way?
                return;
        }

        *cur_inst = *pc;
        uint8_t opcode = NEXT_BYTE;
        
        uint8_t high_nibble = ((opcode >> 4) & 0xF);
        uint8_t low_nibble = (opcode & 0xF);

        // BBSs
        if (high_nibble >= 0x8 &&  high_nibble <= 0xF && low_nibble == 0xF) {
                INST_BBS((high_nibble) - 8); 
                goto TICK_RET;
        }

        // BBRs
        if (high_nibble >= 0x0 &&  high_nibble <= 7 && low_nibble == 0xF) {
                INST_BBR(high_nibble); 
                goto TICK_RET;
        }

        // SMBs
        if (high_nibble >= 0x8 &&  high_nibble <= 0xF && low_nibble == 0x7) {
                INST_SMB((high_nibble) - 8); 
                goto TICK_RET;
        }

        // RMBs
        if (high_nibble >= 0x0 &&  high_nibble <= 0x7 && low_nibble == 0x7) {
                INST_RMB(high_nibble); 
                goto TICK_RET;
        }

        // BITs
        if (opcode == 0x24 || opcode == 0x2C || opcode == 0x89 || opcode == 0x34 || opcode == 0x3C) {
                INST_BIT(opcode);
                goto TICK_RET;
        }

        // Check if opcode is valid
        DBG(0, if (instruction[opcode] == NULL) printf("! Invalid opcode %02X at %04X !\n", opcode, (*pc) - 1);)

        // Consult instruction table
        (*instruction[opcode])();

        TICK_RET:
        cycle_count += cycles[opcode];
}


// Porcessor instructions
void INST_BRK() {
        mem_byte_write(((*pc + 1) >> 8) & 0xFF, 0x100 + ((*register_s)--)); // High
        mem_byte_write((*pc + 1) & 0xFF, 0x100 + ((*register_s)--)); // Low
        INST_PHP(); // Push flags

        // Change flags
        register_p->B = 1; 
        register_p->I = 1;
        register_p->D = 0;

        *pc = PTR(0xFFFE) | (PTR(0xFFFF) << 8);
}
void INST_WAI() { *emulator_flags |= 1 << 7; }
void INST_STP() { *emulator_flags |= 1 << 6; }

// Just NOP
void INST_NOP() { ; }

void reg_dump_65C02() {
        printf("-- 65C02 REG DUMP --\n");
        printf("A     : %02X (%d)\n", *register_a, *register_a);
        printf("X     : %02X (%d)\n", *register_x, *register_x);
        printf("Y     : %02X (%d)\n", *register_y, *register_y);
        printf("S     : %02X (%d)\n", *register_s, *register_s);
        printf("P     : %02X (", *(uint8_t *)register_p);
        if (register_p->N) printf("N");
        if (register_p->V) printf(" | V");
        if (register_p->unused) printf(" | Unused");
        if (register_p->B) printf(" | B");
        if (register_p->D) printf(" | D");
        if (register_p->I) printf(" | I");
        if (register_p->Z) printf(" | Z");
        if (register_p->C) printf(" | C");
        printf(")\n");

        printf("PC    : %04X (%02X", *pc, PTR(*pc));
        if (*pc + 1 < UINT16_MAX) printf(" %02X", PTR(*pc + 1));
        if (*pc + 2 < UINT16_MAX) printf(" %02X", PTR(*pc + 2));
        printf(")\n");

        printf("CYCLE : %X (%ul)\n", cycle_count, cycle_count);

        printf("-- 65C02 REG DUMP END --\n");
}

// Initialize the 6502
void init_65C02() {
        DBG(1, printf("Initializing 65C02 CPU");)

        register_a = (uint8_t *)(memory + REGISTER_A_OFF);
        register_x = (uint8_t *)(memory + REGISTER_X_OFF);
        register_y = (uint8_t *)(memory + REGISTER_Y_OFF);
        register_s = (uint8_t *)(memory + REGISTER_S_OFF);
        pc = (uint16_t *)(memory + REGISTER_PC_OFF);
        register_p = (struct reg_flags *)(memory + REGISTER_P_OFF);
        pins = (uint8_t *)(memory + PINS_OFF);
        emulator_flags = (uint8_t *)(memory + EMU_FLAGS_OFF);
        cur_inst = (uint16_t *)(memory + CUR_INST_OFF);

        ASSERT(register_a != NULL);
        ASSERT(register_x != NULL);
        ASSERT(register_y != NULL);
        ASSERT(register_s != NULL);
        ASSERT(pc != NULL);
        ASSERT(register_p != NULL);
        ASSERT(pins != NULL);
        ASSERT(emulator_flags != NULL);
        ASSERT(cur_inst != NULL);

        *register_a = 0x00;
        *register_x = 0x00;
        *register_y = 0x00;
        *register_s = 0xFF;
        *pc = 0;
        *(uint8_t *)register_p = 0b00110100;
        *pins = 0xE0;
        *emulator_flags = 0x00;

        init_arithmetic_instructions();
        init_comparison_instructions();
        init_control_flow_instructions();
        init_flag_instructions();
        init_inc_dec_instructions();
        init_load_instructions();
        init_logical_instructions();
        init_stack_instructions();
        init_store_instructions();
        init_transfer_instructions();

        // Processor instructions
        instruction[0x00] = INST_BRK; DBG(0, installed_instructions++;)
        instruction[0xDB] = INST_STP; DBG(0, installed_instructions++;)
        instruction[0xCB] = INST_WAI; DBG(0, installed_instructions++;)

        // NOP instruction
        instruction[0xEA] = INST_NOP; DBG(0, installed_instructions++;)

        DBG(1, printf("Initialized 65C02 CPU, %d opcodes installed", installed_instructions + 37);)
}

void destroy_65C02() {
        DBG(1, printf("Destroying 65C02");)
}