#include <video.h>
#include <ram.h>
#include <cpu.h>
#include <SDL2/SDL.h>

#include <keyboard.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Surface* surface = NULL;
SDL_Event event;
uint8_t *video_memory = NULL;
uint8_t *font = NULL;

pthread_t update_vram_thread;
uint16_t sprite_table_address = 0;
struct video_register {
        uint16_t address;
        uint8_t mode;
        uint8_t data;
        uint8_t status; // D(ata)1 R(ead)/W(rite) D(ata)2 B 0 0 0 0
                        // D1: Indicates new data from CPU -> Video card
                        // R/W: 0: reading data, 1: writing data
                        // D2: Indicates new data from Video card -> CPU
                        // B: 0: Draw background, 1: Don't draw background (text mode)
        uint8_t foreground; // Foreground to use for text
        uint8_t background; // Background to use for text (carry set means background is not drawn)

}__attribute__((packed));

uint8_t video_mem_read(uint16_t address) {
        return *(video_memory + address);
}

void video_mem_write(uint16_t address, uint8_t byte) {
        *(video_memory + address) = byte;
}

void *update_vram(void *arg) {
        while (running) {
                struct video_register *reg = (struct video_register *)(general_memory + KERNAL_DAT_BASE); // Temporary "KERNAL_DAT_BASE"
                if ((reg->status >> 7) & 1 == 1) {
                        reg->status &= ~(1 << 7); // Clear D1
                        reg->status &= ~(1 << 5); // Clear D2
                        
                        switch (reg->mode) {
                        case 0x00: // 320x200 8-bit Color
                                // We are outside of the screen, do not write.
                                if (reg->address >= VRAM_SIZE)
                                        break;

                                if ((reg->status >> 6) & 1 == 0) { 
                                        // Read
                                        reg->data = *(video_memory + reg->address);
                                        reg->status |= (1 << 5); // Set D2
                                } else {
                                        // Write
                                        *(video_memory + reg->address) = reg->data;
                                }

                                break;
                        
                        case 0x01: { // Sprite Mode (40x40)
                                // 8 bytes wide, 5 bytes tall
                                uint8_t *data = (uint8_t *)general_memory + (reg->data * SPRITE_WIDTH * SPRITE_HEIGHT + sprite_table_address);

                                int cx = (reg->address % 64) * FONT_WIDTH;
                                int cy = (reg->address / 40) * FONT_HEIGHT;
                                for (int i = 0; i < SPRITE_HEIGHT; i++) {
                                        int rx = 0;
                                        for (int j = 0; j < SPRITE_WIDTH; j++) {
                                                if ((i + cy) * 320 + (rx + cx) >= VRAM_SIZE)
                                                        break;

                                                if ((data[i] >> j) & 1) {
                                                        *(video_memory + (i + cy) * 320 + (rx + cx)) = reg->foreground;
                                                } else if (!((reg->status >> 4) & 1)) {
                                                        *(video_memory + (i + cy) * 320 + (rx + cx)) = reg->background;
                                                }

                                                rx++;
                                        }
                                }

                                break;
                        }

                        case 0x02: // Set Sprite Table Address
                                sprite_table_address = reg->address;
                                break;
                        
                        case 0x03: // 40x12 Terminal Mode
                                // Address behaves like index into screen: y * 40 + x
                                uint8_t* data = font + reg->data * FONT_HEIGHT;

                                int cx = (reg->address % 40) * FONT_WIDTH;
                                int cy = (reg->address / 40) * FONT_HEIGHT;
                                
                                if ((reg->address / 40) > 11)
                                        break;

                                for (int i = 0; i < FONT_HEIGHT; i++) {
                                        int rx = 0;
                                        for (int j = FONT_WIDTH; j >= 0; j--) {
                                                // We are outside of the screen, do not write.
                                                if ((i + cy) * 320 + (rx + cx) >= VRAM_SIZE)
                                                        break;

                                                if ((data[i] >> j) & 1) {
                                                        *(video_memory + (i + cy) * 320 + (rx + cx)) = reg->foreground;
                                                } else if (!((reg->status >> 4) & 1)) {
                                                        *(video_memory + (i + cy) * 320 + (rx + cx)) = reg->background;
                                                }
                                                
                                                rx++;
                                        }
                                }

                                break;
                        }
                }
        }
}

void update() {
        // update();
        SDL_PollEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
                running = 0;
                break;
        case SDL_KEYDOWN:
                keyboard_request(event.key.keysym.scancode);
                break;
        }

        // render();
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

        if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
                printf("Failed to initialize Video\n");
                exit(1);
        }

        window = SDL_CreateWindow("Pyotor Screen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, 0); // Larger window
        SDL_Delay(100);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, 0, 320, 200);

        pthread_create(&update_vram_thread, NULL, update_vram, NULL);

        ASSERT(window != NULL)
        ASSERT(renderer != NULL)
        ASSERT(texture != NULL)
        ASSERT(update_vram_thread != NULL);

        SDL_ShowWindow(window);

        DBG(1, printf("Initialized Video");)
}

void destroy_video() {
        pthread_join(update_vram_thread, NULL);

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(texture);
        free(video_memory);
        free(font);
        SDL_Quit();
}