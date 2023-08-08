#ifndef CGA_H
#define CGA_H

#include "global.h"
#include <stdint.h>

#define VRAM_WIDTH 320
#define VRAM_HEIGHT 200
#define VRAM_SIZE VRAM_WIDTH * VRAM_HEIGHT // Bytes
#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 5
#define TERMINAL_WIDTH 40
#define TERMINAL_REAL_WIDTH (TERMINAL_WIDTH - 1)
#define TERMINAL_HEIGHT 12
#define TERMINAL_REAL_HEIGHT (TERMINAL_HEIGHT - 1)

uint8_t video_mem_read(uint16_t address);
void video_mem_write(uint16_t address, uint8_t byte);

void video_draw_character(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t properties);
void video_draw_sprite(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t properties);
void video_set_sprite_table_address(uint16_t address);
uint16_t video_get_sprite_table_address();
void video_clear_character_buffer();

void update();

void init_video();
void destroy_video();

#endif