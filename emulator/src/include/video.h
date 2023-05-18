#ifndef CGA_H
#define CGA_H

#include <global.h>

#define VRAM_WIDTH 320
#define VRAM_HEIGHT 200
#define VRAM_SIZE VRAM_WIDTH * VRAM_HEIGHT // Bytes
#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 5

uint8_t video_mem_read(uint16_t address);
void video_mem_write(uint16_t address, uint8_t byte);

void video_draw_character(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t draw_fg_bg);
void video_draw_sprite(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t draw_fg_bg);
void video_set_sprite_table_address(uint16_t address);

void update();

void init_video();
void destroy_video();

#endif