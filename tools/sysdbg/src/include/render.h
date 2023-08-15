#ifndef RENDER_H
#define RENDER_H

#include "global.h"
#include "disassemble.h"

int draw_disassembly(uint8_t *flags);
void draw_registers(uint16_t cur_pc, int cur_inst_len);

#endif