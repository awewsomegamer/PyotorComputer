#include "global.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <video.h>
#include <ram.h>
#include <cpu/cpu.h>
#include <keyboard.h>
#include <audio.h>

#include <SDL2/SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Event event;
uint8_t *video_memory = NULL;
uint8_t *character_memory = NULL;
uint8_t *draw_buffer = NULL;
uint8_t *font = NULL;
const int TERMINAL_EXCL_LROW = (TERMINAL_WIDTH * TERMINAL_HEIGHT * 2) - (TERMINAL_WIDTH * 2); // Offset in bytes to the start of the last row in terminal mode

uint16_t sprite_table_address = 0;

uint8_t video_mem_read(uint16_t address) {
        return *(video_memory + address);
}

void video_mem_write(uint16_t address, uint8_t byte) {
        *(video_memory + address) = byte;
}

void video_draw_character(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t properties) {
        int cx = (address & 0xFF);
        int cy = ((address >> 8) & 0xFF);
        
        if (cy > TERMINAL_REAL_HEIGHT && ((properties >> 2) & 1) == 1) {
                int difference = cy - TERMINAL_REAL_HEIGHT;
                
                for (; difference > 0; difference--) {
                        memcpy(character_memory, character_memory + (TERMINAL_WIDTH * 2), TERMINAL_EXCL_LROW);
                        memset(character_memory + TERMINAL_EXCL_LROW, 4, TERMINAL_WIDTH * 2);
                }

                cy = 11;
        }
        
        *(character_memory + (cx + (cy * TERMINAL_WIDTH)) * 2) = data;
        *(character_memory + (cx + (cy * TERMINAL_WIDTH)) * 2 + 1) = properties & 3;
}

void video_draw_sprite(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t properties) {
        uint8_t *spr_data = (uint8_t *)(general_memory + (data * SPRITE_HEIGHT + sprite_table_address));

        int cx = (address & 0xFF) * SPRITE_WIDTH;
        int cy = ((address >> 8) & 0xFF) * SPRITE_HEIGHT;

        for (int i = 0; i < SPRITE_HEIGHT; i++) {
                int rx = 0;
                for (int j = SPRITE_WIDTH - 1; j >= 0; j--) {
                        if ((i + cy) * 320 + (rx + cx) >= VRAM_SIZE)
                                break;

                        if (((spr_data[i] >> j) & 1) && !((properties >> 1) & 1)) {
                                *(video_memory + (i + cy) * 320 + (rx + cx)) = foreground;
                        } else if (!((spr_data[i] >> j) & 1) && !(properties & 1)) {
                                *(video_memory + (i + cy) * 320 + (rx + cx)) = background;
                        }

                        rx++;
                }
        }
}

void video_set_sprite_table_address(uint16_t address) {
        sprite_table_address = address;
}

uint16_t video_get_sprite_table_address() {
        return sprite_table_address;
}

void video_clear_character_buffer() {
        memset(character_memory, 4, TERMINAL_WIDTH * TERMINAL_HEIGHT * 2);
}

void update() {
        // Update
        SDL_PollEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
                DBG(1, printf("Quitting");)
                running = 0;
                break;
        case SDL_KEYDOWN:
                keyboard_request(event.key.keysym.scancode);
                break;
        }

        // Render
        memcpy(draw_buffer, video_memory, VRAM_SIZE);

        for (int x = 0; x < TERMINAL_WIDTH * TERMINAL_HEIGHT; x++) {
                uint8_t properties = *(character_memory + x * 2 + 1);

                if (properties == 4)
                        continue;

                uint8_t data = *(character_memory + x * 2);
                uint8_t *chr_data = font + data * FONT_HEIGHT;
                int cx = (x % TERMINAL_WIDTH) * FONT_WIDTH;
                int cy = (x / TERMINAL_WIDTH) * FONT_HEIGHT;

                for (int i = 0; i < FONT_HEIGHT; i++) {
                        int rx = 0;
                        for (int j = FONT_WIDTH - 1; j >= 0; j--) {
                                // We are outside of the screen, do not write.
                                if ((i + cy) * 320 + (rx + cx) >= VRAM_SIZE)
                                        break;

                                if (((chr_data[i] >> j) & 1) && !((properties >> 1) & 1)) {
                                        *(draw_buffer + (i + cy) * VRAM_WIDTH + (rx + cx)) = *(general_memory + 48517);
                                } else if (!((chr_data[i] >> j) & 1) && !(properties & 1)) {
                                        *(draw_buffer + (i + cy) * VRAM_WIDTH + (rx + cx)) = *(general_memory + 48518);
                                }
                                
                                rx++;
                        }
                }
        }

        SDL_UpdateTexture(texture, NULL, (void *)draw_buffer, 320);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
}

void init_video() {
        DBG(1, printf("Initializing Video");)

        video_memory = (uint8_t *)malloc(VRAM_SIZE);
        character_memory = (uint8_t *)malloc(TERMINAL_WIDTH * TERMINAL_HEIGHT * 2);
        memset(character_memory, 4, TERMINAL_WIDTH * TERMINAL_HEIGHT * 2);
        draw_buffer = (uint8_t *)malloc(VRAM_SIZE);

        ASSERT(video_memory != NULL)
        ASSERT(character_memory != NULL)
        ASSERT(draw_buffer != NULL)

        FILE *font_file = fopen("FONT.bin", "r");

        fseek(font_file, 0, SEEK_END);
        long font_size = ftell(font_file);
        fseek(font_file, 0, SEEK_SET);

        ASSERT(font_size != 0);

        font = (uint8_t *)malloc(font_size);

        ASSERT(font != NULL);

        fread(font, 1, font_size, font_file);

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0 ) {
                printf("SDL failed to initialize Video or Audio\n");
                exit(1);
        }

        window = SDL_CreateWindow("Pyotor Screen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, 0); // Larger window
        SDL_Delay(100);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, 0, 320, 200);

        ASSERT(window != NULL)
        ASSERT(renderer != NULL)
        ASSERT(texture != NULL)

        SDL_ShowWindow(window);

        DBG(1, printf("Initialized Video");)

        init_speaker();
}

void destroy_video() {
        DBG(1, printf("Destroying Video");)

        SDL_DestroyRenderer(renderer);
        SDL_DestroyTexture(texture);
        SDL_DestroyWindow(window);
        // Getting abort here
        // corrupted size vs. prev_size
        free(video_memory);
        free(font);
        free(character_memory);
        free(draw_buffer);

        destroy_speaker();

        SDL_Quit();
}