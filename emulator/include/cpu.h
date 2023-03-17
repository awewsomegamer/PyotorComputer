#ifndef CPU_H
#define CPU_H

#include <global.h>

#define IO_VIDEO_MASK 1

extern uint8_t io_port;

#define OFF_X(value) value + register_x
#define OFF_Y(value) value + register_y
#define PTR(value) (mem_byte_read(value))
#define NEXT_BYTE mem_byte_read(pc++)
#define CUR_BYTE mem_byte_read(pc)
#define NEXT_WORD NEXT_BYTE | (NEXT_BYTE << 8)
#define CMP_SET(v1, v2) \
        { \
                uint32_t iv1 = (uint32_t)v1; \
                uint32_t iv2 = (uint32_t)v2; \
                if      (iv1 < iv2)  { register_p.N = ((iv1 - iv2) >> 7) & 1; register_p.Z = 0; register_p.C = 0; } \
                else if (iv1 == iv2) { register_p.N = 0;                      register_p.Z = 1; register_p.C = 1; } \
                else if (iv1 > iv2)  { register_p.N = ((iv1 - iv2) >> 7) & 1; register_p.Z = 0; register_p.C = 1; } \
        }


void tick_6502();
void reg_dump_6502();
void init_6502();

#endif