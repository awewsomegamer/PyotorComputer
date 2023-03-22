#include <global.h>
#include <cpu.h>
#include <ram.h>
#include <cga.h>
#include <unistd.h>

int main(int argc, char **argv) {
        init_65C02();
        init_ram();
        init_cga();

        FILE *kern_bin = fopen("bin/kernel.bin", "r");
        FILE *boot_bin = fopen("bin/boot.bin", "r");
        
        ASSERT(kern_bin != NULL);
        ASSERT(boot_bin != NULL);

        load_file(KERNEL_MEM_BASE, kern_bin, "bin/kernel.bin");
        load_file(ZERO_PAGE_BASE, boot_bin, "bin/boot.bin");

        for (;;) {
                reg_dump_65C02();
                printf("MEMORY @ 0x1FF %X\n", mem_byte_read(0x1FF));
                printf("MEMORY @ 0x200 %X\n", mem_byte_read(0x200));
                printf("MEMORY @ 0x201 %X\n", mem_byte_read(0x201));
                tick_65C02();
        }

        return 0;
}