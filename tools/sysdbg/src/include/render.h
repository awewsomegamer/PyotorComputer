#ifndef RENDER_H
#define RENDER_H

#include "global.h"
#include "disassemble.h"

int draw_disassembly();
void draw_registers(uint16_t cur_pc, int cur_inst_len);
void draw_sys_info();
void render_cmd();

#endif