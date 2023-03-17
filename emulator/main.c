#include <global.h>
#include <cpu.h>
#include <ram.h>

int main(int argc, char **argv) {
        init_6502();
        init_ram();

        FILE *bin = fopen("test.bin", "r");
        
        ASSERT(bin != NULL);

        load_file(0x0, bin);

        for (;;) {
                reg_dump_6502();
                printf("MEMORY @ 0x1FF %X\n", mem_byte_read(0x1FF));
                printf("MEMORY @ 0x200 %X\n", mem_byte_read(0x200));
                printf("MEMORY @ 0x201 %X\n", mem_byte_read(0x201));
                tick_6502();
        }

        return 0;
}