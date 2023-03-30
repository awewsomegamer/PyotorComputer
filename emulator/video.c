#include <video.h>
#include <ram.h>
#include <SDL2/SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Surface* surface = NULL;
SDL_Event event;
uint8_t *video_memory = NULL;

pthread_t update_vram_thread;
struct video_register {
        uint32_t address : 24;
        uint8_t data;
        uint8_t status; // D(ata)1 R(ead)/W(rite) D(ata)2 0 0 0 0 0
                        // D1: Indicates new data from CPU -> Video card
                        // R/W: 0: reading data, 1: writing data
                        // D2: Indicates new data from Video card -> CPU

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
                        
                        if (reg->address >= VRAM_SIZE)
                                continue;

                        if ((reg->status >> 6) & 1 == 0) { 
                                // Read
                                reg->data = *(video_memory + reg->address);
                                reg->status |= (1 << 5); // Set D2
                        } else {
                                // Write
                                *(video_memory + reg->address) = reg->data;
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
        SDL_Quit();
}