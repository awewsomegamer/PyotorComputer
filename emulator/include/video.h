#ifndef CGA_H
#define CGA_H

#include <global.h>

#define VRAM_WIDTH 320
#define VRAM_HEIGHT 200
#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define VRAM_SIZE VRAM_WIDTH * VRAM_HEIGHT // Bytes

uint8_t video_mem_read(uint16_t address);
void video_mem_write(uint16_t address, uint8_t byte);

void update();

void init_video();
void destroy_video();

#endif