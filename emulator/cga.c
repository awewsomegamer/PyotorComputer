#include <cga.h>
#include <ram.h>

#include <SDL2/SDL.h>

uint8_t *cga_memory = NULL;

uint8_t cga_mem_read(uint16_t address) {
        return *(cga_memory + address);
}

void cga_mem_write(uint16_t address, uint8_t byte) {
        *(cga_memory + address) = byte;
}

void *render_loop(void *args) {

}

void init_cga() {
        DBG(1, printf("Initializing CGA");)

        cga_memory = (uint8_t *)malloc(KB_16);
        
        DBG(1, printf("Initialized CGA");)
}