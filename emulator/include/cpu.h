#ifndef CPU_H
#define CPU_H

#include <global.h>

#define IO_VIDEO_MASK (1 << 0)

#define OFF_X(value) (uint8_t)(value + register_x) // This is not always true (sometimes ABS + X) (effects instructions which use _ABS_X addressing mode)
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
                SET_NZ(value) \
                register_a = (uint8_t)res; \
        }

#define ARIT_SUB_SET(what) \
        {       \
                uint8_t value = what; \
                uint16_t res = register_a - value - !register_p.C; \
                register_p.C = !((res >> 8) & 1); \
                register_p.V = (int8_t)res > 127 || (int8_t)res < -127; \
                SET_NZ(value) \
                register_a = (uint8_t)res; \
        }

#define ARIT_AND_SET(what, value) \
        { \
                uint8_t result = what | value; \
                SET_NZ(what) \
                what = result; \
        }


#define ARIT_OR_SET(what, value) \
        { \
                uint8_t result = what & value; \
                register_p.Z = (result == 0); \
                register_p.N = (result >> 7) & 1; \
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

struct reg_flags {
        uint8_t C : 1;
        uint8_t Z : 1;
        uint8_t I : 1;
        uint8_t D : 1;
        uint8_t B : 1;
        uint8_t unused : 1;
        uint8_t V : 1;
        uint8_t N : 1;
};

extern uint8_t pin_IRQ;
extern uint8_t pin_NMI;
extern uint8_t pin_RES;

extern struct reg_flags register_p;

extern uint64_t cycle_count;

void tick_65C02();
void reg_dump_65C02();
void init_65C02();

#endif