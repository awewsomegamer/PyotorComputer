#include <global.h>
#include <cpu.h>
#include <ram.h>
#include <cga.h>
#include <unistd.h>
#include <time.h>

#define SYSCLOCK_SPEED_MHZ 10
#define SYS_MICRO_SPEED 1 / SYSCLOCK_SPEED_MHZ

int main(int argc, char **argv) {
        init_65C02();
        init_ram();
        init_cga();

        FILE *kern_bin = fopen("bin/kernal.bin", "r");
        
        ASSERT(kern_bin != NULL);

        load_file(KERNAL_MEM_BASE, kern_bin, "bin/kernel.bin");

        pin_RES = 0;

        time_t last_second = time(NULL);
        uint64_t ticks = 0;
        uint64_t cycle_delta_sum = 0;
        for (;;) {
                reg_dump_65C02();

                uint64_t cycle_start = cycle_count;
                tick_65C02();
                ticks++;
                cycle_delta_sum += cycle_count - cycle_start;
                
                if (cycle_delta_sum >= SYSCLOCK_SPEED_MHZ) {
                        usleep(SYS_MICRO_SPEED);
                        cycle_delta_sum -= SYSCLOCK_SPEED_MHZ;
                }
        
                if (time(NULL) - last_second == 1) {
                        printf("%d Ticks, %d cycles \n", ticks, cycle_count);
                        ticks = 0;
                        cycle_count = 0;
                        last_second = time(NULL);
                }

                // reg_dump_65C02();

                sleep(1);
        }
        
        return 0;
}