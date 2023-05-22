#include <keyboard.h>
#include <cpu/cpu.h>
#include <ram.h>

void keyboard_request(uint8_t scan) {
        DBG(1, printf("Key 0x%02X requested", scan);)

        *(general_memory + 0x3) = scan;

        pin_IRQ = 0;
}
