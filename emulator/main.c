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

        pin_RES = 0;

        for (;;) {
                tick_65C02();
                reg_dump_65C02();
                sleep(1);
        }
        
        return 0;
}