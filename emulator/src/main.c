#include <global.h>
#include <cpu/cpu.h>
#include <ram.h>
#include <video.h>
#include <control_reg.h>
#include <disk.h>

#define SYSCLOCK_SPEED_MHZ 10
#define SYS_MICRO_SPEED 1 / SYSCLOCK_SPEED_MHZ

uint8_t running = 1;

void *emulate(void *arg) {
        time_t last_second = time(NULL);
        uint64_t ticks = 0;
        uint64_t cycle_delta_sum = 0;
        while (running) {
                // reg_dump_65C02();

                uint64_t cycle_start = cycle_count;
                tick_65C02();
                ticks++;
                cycle_delta_sum += cycle_count - cycle_start;
                
                if (cycle_delta_sum >= SYSCLOCK_SPEED_MHZ) {
                        usleep(SYS_MICRO_SPEED);
                        cycle_delta_sum -= SYSCLOCK_SPEED_MHZ;
                }
        
                if (time(NULL) - last_second == 1) {
                        // printf("%d Ticks, %d cycles \n", ticks, cycle_count);
                        ticks = 0;
                        cycle_count = 0;
                        last_second = time(NULL);
                }

                tick_control_register();
        }

        DBG(1, printf("Emulation thread finished execution");)

        return NULL;
}

int main(int argc, char **argv) {
        init_65C02();
        init_ram();
        init_video();

        connect_disk("disk1.bin", 0);

        load_file(KERNAL_MEM_BASE, "bin/kernal.bin");
        
        pin_RES = 0;

        pthread_t emulation_thread;
        pthread_create(&emulation_thread, NULL, emulate, NULL);

        while (running)
                update();
        
        pthread_join(emulation_thread, NULL);
        destroy_video();
        destroy_65C02();
        disconnect_all();

        DBG(1, printf("Emulator stopped");)

        return 0;
}
