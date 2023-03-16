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

void tick_6502();
void reg_dump_6502();
void init_6502();

#endif