#include <SDL2/SDL_timer.h>
#include "include/kbd.h"
#include "include/cpu/cpu.h"
#include "include/ram.h"

void keyboard_request(uint8_t scan) {
        DBG(1, printf("Key 0x%02X requested", scan);)

        *(memory + 0x3) = scan;

        *pins &= ~(1 << 7);
}
