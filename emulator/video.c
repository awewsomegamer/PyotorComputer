#include <video.h>
#include <ram.h>
#include <string.h>
#include <SDL2/SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Surface* surface = NULL;
pthread_t window_loop_thread;
SDL_Event event;
uint8_t *video_memory = NULL;

uint8_t video_mem_read(uint16_t address) {
        return *(video_memory + address);
}

void video_mem_write(uint16_t address, uint8_t byte) {
        *(video_memory + address) = byte;
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
        memcpy(surface->pixels, video_memory, sizeof(video_memory));
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
}

void init_video() {
        DBG(1, printf("Initializing Video");)

        video_memory = (uint8_t *)malloc(KB_16);

        if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
                printf("Failed to initialize Video\n");
                exit(1);
        }


        window = SDL_CreateWindow("Pyotor Screen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
        renderer = SDL_CreateRenderer(window, -1, 0);
        surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 4, 0, 0, 0, 0);

        SDL_Color colors[2] = {{0, 0, 255, 255}, {255, 0, 0, 255}};
        SDL_SetPaletteColors(surface, colors, 0, 2);

        SDL_ShowWindow(window);

        DBG(1, printf("Initialized Video");)
}

void destroy_video() {
        pthread_join(window_loop_thread, NULL);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(texture);
        free(video_memory);
        SDL_Quit();
}