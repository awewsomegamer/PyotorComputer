#ifndef CGA_H
#define CGA_H

#include <global.h>

uint8_t cga_mem_read(uint16_t address);
void cga_mem_write(uint16_t address, uint8_t byte);

void init_cga();

#endif