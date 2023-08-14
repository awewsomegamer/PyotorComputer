#include <SDL2/SDL_timer.h>
#include <ctype.h>
#include "include/global.h"
#include "include/cpu/cpu.h"
#include "include/ram.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "include/video.h"
#include "include/ctrl_reg.h"
#include "include/disk.h"
#include "include/scheduler.h"
#include "include/shared_memory.h"

#ifndef SYS_IPS
#define SYS_IPS 3500000 // Default rated number of instructions per second for a modern 65C02
#endif

uint8_t running = 1;
double *sys_ips;

void *emulate_thread(void *arg) {
        time_t last_second = time(NULL);
        uint64_t instructions = 0;
        uint64_t last_tick_base = SDL_GetTicks64();
        const double wait_between_insts = (double)1/(double)SYS_IPS;
        double current_debt = 0;
        double threshold = 0.0001;
        
        *emulator_flags |= 1 << 5;

        while (running) {
                tick_65C02();
                current_debt += wait_between_insts;
                instructions++;

                tick_control_register();

                if (current_debt >= threshold) {
                        usleep(1);
                        current_debt = 0;
                }

                if (time(NULL) - last_second == 1) {
                        DBG(1, printf("%d IPS, %d Cycles, Threshold (%d): %f", instructions, cycle_count, (instructions > *sys_ips), threshold);)
                        
                        // Try and adjust the threshold to better approximate desired speed
                        if (instructions > *sys_ips)
                                threshold -= 0.000001;
                        else if (instructions < *sys_ips)
                                threshold += 0.000001;

                        instructions = 0;
                        cycle_count = 0;
                        last_tick_base = SDL_GetTicks64();
                        last_second = time(NULL);

                }
        }

        DBG(1, printf("Emulation thread finished execution");)

        return NULL;
}

int main(int argc, char **argv) {
        init_shared_memory_host();

        sys_ips = (double *)(memory + IPS_OFF);
        *sys_ips = SYS_IPS;

        init_65C02();
        init_video();

        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-disk") == 0) {
                        // -disk disk_number disk_file
                        connect_disk(argv[i + 2], atoi(argv[i + 1]));
                }

                if (strcmp(argv[i], "-load") == 0) {
                        // -load mem mem_file
                        // mem: can be an address (i.e. 0xDCC0) which must be expressed in hex (prefixed with 0x)
                        uint16_t address = 0;

                        if (argv[i + 1][0] == '0' && toupper(argv[i + 1][1]) == 'X') {
                                address = strtol(argv[i + 1], NULL, 16);
                        } else {
                                printf("Couldn't load file %s to address %s, not a valid hexadecimal value.\n", argv[i + 2], argv[i + 1]);
                                exit(1);
                        }
                        
                        load_file(address, argv[i + 2]);
                }
        }        

        *pins &= ~(1 << 5);

        pthread_t emulation_thread_t;
        pthread_create(&emulation_thread_t, NULL, emulate_thread, NULL);

        while (running) {
                update();
                do_task();
        }
        
        pthread_join(emulation_thread_t, NULL);
        destroy_video();
        destroy_65C02();
        disconnect_all();
        destroy_shared_memory_host();

        DBG(1, printf("Emulator stopped");)

        return 0;
}
