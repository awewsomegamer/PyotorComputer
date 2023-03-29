#ifndef CGA_H
#define CGA_H

#include <global.h>

uint8_t video_mem_read(uint16_t address);
void video_mem_write(uint16_t address, uint8_t byte);

void update();

void init_video();
void destroy_video();

#endif