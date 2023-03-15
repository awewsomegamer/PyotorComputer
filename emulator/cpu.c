#include <cpu.h>

// Initial states of registers
uint8_t io_port = 0;
uint8_t register_a = 0;
uint8_t register_x = 0;
uint8_t register_y = 0;
uint8_t register_s = 0xFF;
struct {
        uint8_t C : 1;
        uint8_t Z : 1;
        uint8_t I : 1;
        uint8_t D : 1;
        uint8_t B : 1;
        uint8_t unused : 1;
        uint8_t V : 1;
        uint8_t N : 1;
} register_p;

// Preform one cycle on the 6502
void tick_6502() {

}

// Initialize the 6502
void init_6502() {
        DBG(1, "Initializing 6502 CPU")

        DBG(1, "Initialized 6502 CPU")
}