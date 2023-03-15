#ifndef RAM_H
#define RAM_H

#include <global.h>

#define ZERO_PAGE(a) 0x0000 <= a <= 0x00FF
#define STACK(a)     0x0100 <= a <= 0x01FF
#define VIDEO_MEM(a) 0x0200 <= a <= 0x4080

uint8_t mem_byte_read(uint16_t);
void mem_byte_write(uint8_t, uint16_t);

void init_ram();

#endif