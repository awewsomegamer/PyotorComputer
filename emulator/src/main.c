#include <SDL2/SDL_timer.h>
#include <ctype.h>
#include <global.h>
#include <cpu/cpu.h>
#include <ram.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <video.h>
#include <ctrl_reg.h>
#include <disk.h>

#ifndef SYS_IPS
#define SYS_IPS 3500000 // Default rated number of instructions per second for a modern 65C02
#endif

uint8_t running = 1;

void *emulate(void *arg) {
        time_t last_second = time(NULL);
        uint64_t instructions = 0;
        uint64_t last_tick_base = SDL_GetTicks64();
        const double wait_between_insts = (double)1/(double)SYS_IPS;
        double current_debt = 0;

        while (running) {
                tick_65C02();
                current_debt += wait_between_insts;
                tick_control_register();
                instructions++;
                
                if (current_debt >= 0.0001) {
                        usleep(1);
                        current_debt = 0;
                }
                
                if (time(NULL) - last_second == 1) {
                        DBG(1, printf("%d IPS %d Cycles", instructions, cycle_count);)
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
        init_65C02();
        init_ram();
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
