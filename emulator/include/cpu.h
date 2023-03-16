#ifndef CPU_H
#define CPU_H

#include <global.h>

#define IO_VIDEO_MASK 1

extern uint8_t io_port;

void tick_6502();
void reg_dump_6502();
void init_6502();

#endif