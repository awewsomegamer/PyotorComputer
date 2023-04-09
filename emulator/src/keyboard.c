#include <keyboard.h>
#include <cpu/cpu.h>
#include <ram.h>

void keyboard_request(uint8_t scan) {
        printf("%X\n", scan);
    
        *(general_memory + 0x3) = scan;
        *(general_memory + 0x4) |= 0x1;

        pin_IRQ = 0;
}
