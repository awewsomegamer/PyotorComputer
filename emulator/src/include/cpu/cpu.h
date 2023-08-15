#ifndef CPU_H
#define CPU_H

#include "../global.h"

#define IO_VIDEO_MASK (1 << 0)

extern uint8_t *register_a;
extern uint8_t *register_x;
extern uint8_t *register_y;
extern uint8_t *register_s;

struct reg_flags {
        uint8_t C : 1;
        uint8_t Z : 1;
        uint8_t I : 1;
        uint8_t D : 1;
        uint8_t B : 1;
        uint8_t unused : 1;
        uint8_t V : 1;
        uint8_t N : 1;
}__attribute__((packed));

extern struct reg_flags *register_p;
extern uint16_t *pc;

extern uint8_t *pins;
extern uint8_t *emulator_flags;

extern uint64_t cycle_count;

extern void (*instruction[0xFF])();
extern int installed_instructions;

void tick_65C02();
void reg_dump_65C02();
void init_65C02();
void destroy_65C02();

#endif