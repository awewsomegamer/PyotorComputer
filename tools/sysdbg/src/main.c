#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global.h"
#include "include/render.h"
#include "include/shared_memory.h"
#include "include/disassemble.h"
#include <assert.h>

int max_x = 0;
int max_y = 0;

void init_ncurses() {
	setlocale(LC_ALL, "en_US.UTF-8");

        initscr();
        cbreak();
        noecho();

        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
	
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

int main(int argc, char **argv) {
        FILE *code;
        FILE *labels;

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

        init_shared_memory_client();
        init_ncurses();

        uint8_t flags = 0b00000000;

        // While running
        uint16_t cur_pc = 0;
        while ((*(memory + EMU_FLAGS_OFF) >> 5) & 1) {
                cur_pc = *(memory + CUR_INST_OFF) | (*(memory + CUR_INST_OFF + 1) << 8);
                pc = cur_pc;

                int cur_inst_len = draw_disassembly(&flags);
                draw_registers(cur_pc, cur_inst_len);

                refresh();
                erase();
        }

        endwin();

        destroy_shared_memory_client();


	return 0;
}