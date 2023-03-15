#include <global.h>
#include <cpu.h>
#include <ram.h>

int main(int argc, char **argv) {
        init_6502();
        init_ram();

        printf("[0x0] : %2X\n", mem_byte_read(0x0));
        mem_byte_write(0xFF, 0x0);
        printf("[0x0] : %2X\n", mem_byte_read(0x0));


        return 0;
}