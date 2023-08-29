#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global.h"
#include "include/render.h"
#include "include/shared_memory.h"
#include "include/disassemble.h"
#include "include/input.h"
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

int max_x = 0;
int max_y = 0;
uint8_t debugger_mode = 0;

uint8_t running = 1;

pthread_t poll_thread;

void init_ncurses() {
	setlocale(LC_ALL, "en_US.UTF-8");

        initscr();
        cbreak();
        noecho();

        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
	
	getmaxyx(stdscr, max_y, max_x);

        if (max_x == 0 || max_y == 0) {
                // Screen too small
                endwin();
                printf("Terminal too small\n");
                exit(1);
        }

        if (has_colors() == TRUE) {
                start_color();
                use_default_colors();

                init_pair(SYSDBG_COLOR_HIGHLIGHTED, COLOR_BLACK, COLOR_WHITE);
                init_pair(SYSDBG_COLOR_GREEN, COLOR_BLACK, COLOR_GREEN);
                init_pair(SYSDBG_COLOR_RED, COLOR_BLACK, COLOR_RED);
        }
}

// Get a better per character hash function
size_t char_hash(char c) {
        return (((0x55555 * c) / 0xADAFB) ^ 2) / 8;
}

void terminate(int sig) {
        signal(sig, SIG_IGN);
        running = 0;
}

int main(int argc, char **argv) {
        FILE *code;
        FILE *labels;

        signal(SIGINT, terminate);

        if (argc > 1 && strcmp(argv[1], "-d") == 0) {
                uint8_t flags = 0b00000000;

                if (argc < 2) {
                        printf("Disassembly mode:\nmonitor -d assembled.bin [labels.txt] [$binary_origin]\n");
                        return 1;
                }

                code = fopen(argv[2], "r");

                if (argc > 3) {
                        labels = fopen(argv[3], "r");
                        parse_labels(labels);
                        fclose(labels);
                }

                if (argc > 4) {
                        flags |= 1 << 1;
                        code_org = strtol(argv[4], NULL, 16);
                }

                assert(code != NULL);

                fseek(code, 0, SEEK_END);
                size_t size = ftell(code);
                fseek(code, 0, SEEK_SET);
                
                uint8_t *buffer = malloc(size);
                fread(buffer, 1, size, code);

                while (pc < size) {
                        char *str = print_instruction(buffer, &flags);

                        if (flags & 1)
                                printf("%s:\n", str);
                        else
                                printf("\t%s\n", str);
                        
                        free(str);
                }

                return 0;
        }

        if (argc > 1) {
                labels = fopen(argv[1], "r");
                parse_labels(labels);
                fclose(labels);
        }

        // Implement support for a breakpoints file.
        // Also the above argc interpreting should be
        // revised.

        init_shared_memory_client();
        init_ncurses();

        uint8_t flags = 0b00001000;
        
        // While running
        uint16_t cur_pc = 0;
        while (running && ((*(memory + EMU_FLAGS_OFF) >> 5) & 1)) {
                shared_memory_acquire_lock();
                
                cur_pc = *(memory + CUR_INST_OFF) | (*(memory + CUR_INST_OFF + 1) << 8);
                pc = cur_pc;

                int cur_inst_len = draw_disassembly(&flags);
                draw_registers(cur_pc, cur_inst_len);
                draw_sys_info();
                
                shared_memory_release_lock();
                
                int c = 0;
                if ((c = getch()) != ERR)
                        interpret_key(c);

                refresh();

                usleep(1000);
                erase();
        }

        clear();
        endwin();

        if (lock_owned)
                shared_memory_release_lock();

	return 0;
}