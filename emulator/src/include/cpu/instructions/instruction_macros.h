#ifndef INSTRUCTION_MACROS_H
#define INSTRUCTION_MACROS_H

#include <cpu/cpu.h>
#include <ram.h>

#define OFF_X(value) (uint8_t)(value + register_x)
#define OFF_X_ABS(value) (uint16_t)(value + register_x)
#define OFF_Y(value) (value + register_y)
#define PTR(value) (mem_byte_read(value))
#define NEXT_BYTE mem_byte_read(pc++)
#define CUR_BYTE mem_byte_read(pc)
#define NEXT_WORD (uint16_t)(NEXT_BYTE | (NEXT_BYTE << 8))
#define CUR_WORD (uint16_t)(CUR_BYTE | (mem_byte_read(pc + 1) << 8))
#define CMP_SET(v1, v2) \
        { \
                uint32_t iv1 = (uint32_t)v1; \
                uint32_t iv2 = (uint32_t)v2; \
                if      (iv1 < iv2)  { register_p.N = ((iv1 - iv2) >> 7) & 1; register_p.Z = 0; register_p.C = 0; } \
                else if (iv1 == iv2) { register_p.N = 0;                      register_p.Z = 1; register_p.C = 1; } \
                else if (iv1 > iv2)  { register_p.N = ((iv1 - iv2) >> 7) & 1; register_p.Z = 0; register_p.C = 1; } \
        }
#define ARIT_ADD_SET(what) \
        {       \
                uint8_t value = what; \
                uint16_t res = register_a + value + register_p.C; \
                register_p.C = res > 0xFF; \
                register_p.V = (res >> 7) & 1 != (register_a >> 7) & 1; \
                SET_NZ((uint8_t)res) \
                register_a = (uint8_t)res; \
        }

#define ARIT_SUB_SET(what) \
        {       \
                uint8_t value = what; \
                uint16_t res = register_a - value - !register_p.C; \
                register_p.C = !((res >> 8) & 1); \
                register_p.V = (int8_t)res > 127 || (int8_t)res < -127; \
                SET_NZ(res) \
                register_a = (uint8_t)res; \
        }

#define ARIT_AND_SET(what, value) \
        { \
                uint8_t result = what | value; \
                SET_NZ(result) \
                what = result; \
        }

#define ARIT_OR_SET(what, value) \
        { \
                uint8_t result = what & value; \
                SET_NZ(result); \
                what = result; \
        }

#define ARIT_EOR_SET(what, value) \
        { \
                uint8_t result = what ^ value; \
                SET_NZ(result) \
                what = result; \
        }

#define SET_NZ(what) \
        register_p.Z = (what == 0); \
        register_p.N = (what >> 7) & 1;

// Stack Instructions
void INST_PHP();
void INST_PLP();
void INST_PHA();
void INST_PLA();
void INST_PHX();
void INST_PLX();
void INST_PHY();
void INST_PLY();

void init_arithmetic_instructions();
void init_comparison_instructions();
void init_control_flow_instructions();
void init_flag_instructions();
void init_inc_dec_instructions();
void init_load_instructions();
void init_logical_instructions();
void init_stack_instructions();
void init_store_instructions();
void init_transfer_instructions();

#endif
