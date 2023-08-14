#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/disassemble.h"
#include <assert.h>

#define NUM_INST_TO_DISASSEMBLE max_y - 1

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

        max_y--;
        max_x--;

        if (max_x == 0 || max_y == 0) {
                // Screen too small
                exit(1);
        }

        addch('A');
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
        while ((*(memory + EMU_FLAGS_OFF) >> 5) & 1) {
                pc = *(memory + REGISTER_IP_OFF) | (*(memory + REGISTER_IP_OFF + 1) << 8);

                for (int i = 0; i < NUM_INST_TO_DISASSEMBLE; i++) {
                        char *str = print_instruction(memory, &flags);

                        move(i, 40);

                        if (flags & 1)
                                move(i, 16);

                        printw("%s%c", str, (flags & 1 ? ':' : 0));
                        free(str);

                        if (flags & 1) {
                                str = print_instruction(memory, &flags);
                                move(i, 40);
                                printw("%s", str);
                                free(str);
                        }
                }

                refresh();
                erase();
        }

        endwin();

        destroy_shared_memory_client();


	return 0;
}