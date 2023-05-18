#include "global.h"
#include <stdio.h>
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
uint8_t *font = NULL;

uint16_t sprite_table_address = 0;

uint8_t video_mem_read(uint16_t address) {
        return *(video_memory + address);
}

void video_mem_write(uint16_t address, uint8_t byte) {
        *(video_memory + address) = byte;
}

void video_draw_character(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t draw_fg_bg) {
        uint8_t* chr_data = font + data * FONT_HEIGHT;

        int cx = (address % 40) * FONT_WIDTH;
        int cy = (address / 40) * FONT_HEIGHT;
        
        if ((address / 40) > 11)
                return;

        for (int i = 0; i < FONT_HEIGHT; i++) {
                int rx = 0;
                for (int j = FONT_WIDTH; j >= 0; j--) {
                        // We are outside of the screen, do not write.
                        if ((i + cy) * 320 + (rx + cx) >= VRAM_SIZE)
                                break;

                        if (((chr_data[i] >> j) & 1) && !((draw_fg_bg >> 1) & 1)) {
                                *(video_memory + (i + cy) * 320 + (rx + cx)) = foreground;
                        } else if (!((chr_data[i] >> j) & 1) && !(draw_fg_bg & 1)) {
                                *(video_memory + (i + cy) * 320 + (rx + cx)) = background;
                        }
                        
                        rx++;
                }
        }
}

void video_draw_sprite(uint16_t address, uint8_t data, uint8_t foreground, uint8_t background, uint8_t draw_fg_bg) {
        uint8_t *spr_data = (uint8_t *)general_memory + (data * SPRITE_WIDTH * SPRITE_HEIGHT + sprite_table_address);

        int cx = (address % 40) * FONT_WIDTH;
        int cy = (address / 40) * FONT_HEIGHT;
        for (int i = 0; i < SPRITE_HEIGHT; i++) {
                int rx = 0;
                for (int j = 0; j < SPRITE_WIDTH; j++) {
                        if ((i + cy) * 320 + (rx + cx) >= VRAM_SIZE)
                                break;

                        if (((spr_data[i] >> j) & 1) && !((draw_fg_bg >> 1) & 1)) {
                                *(video_memory + (i + cy) * 320 + (rx + cx)) = foreground;
                        } else if (!((spr_data[i] >> j) & 1) && !(draw_fg_bg & 1)) {
                                *(video_memory + (i + cy) * 320 + (rx + cx)) = background;
                        }

                        rx++;
                }
        }
}

void video_set_sprite_table_address(uint16_t address) {
        sprite_table_address = address;
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
        SDL_UpdateTexture(texture, NULL, (void *)video_memory, 320);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
}

void init_video() {
        DBG(1, printf("Initializing Video");)

        video_memory = (uint8_t *)malloc(VRAM_SIZE);

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
        free(video_memory);
        free(font);

        destroy_speaker();

        SDL_Quit();
}