#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/disassemble.h"
#include <assert.h>

#define LABEL_COLUMN 0
#define CODE_COLUMN (max_x / 3)
#define CODE_COLUMN_END (max_x * 3/4)
#define DISASM_START_ROW ((max_y / 2) - 1)

#define MON_COLOR_REGULAR 0
#define MON_COLOR_HIGHLIGHTED 1
#define MON_COLOR_GREEN 2
#define MON_COLOR_RED 3

#define HIGHLIGHT_ON if ((has_colors() == TRUE) && (i == DISASM_START_ROW)) \
                                attron(COLOR_PAIR(MON_COLOR_HIGHLIGHTED));
#define HIGHLIGHT_OFF if ((has_colors() == TRUE) && (i == DISASM_START_ROW)) \
                                attroff(COLOR_PAIR(MON_COLOR_HIGHLIGHTED));

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
                
                init_pair(MON_COLOR_HIGHLIGHTED, COLOR_BLACK, COLOR_WHITE);
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
        while ((*(memory + EMU_FLAGS_OFF) >> 5) & 1) {
                pc = *(memory + CUR_INST_OFF) | (*(memory + CUR_INST_OFF + 1) << 8);

                for (int i = DISASM_START_ROW; i < max_y; i++) {
                        char *str = print_instruction(memory, &flags);
                        char *printable_str = malloc(max_x + 1);
                        memset(printable_str, 0, max_x + 1);

                        int length = CODE_COLUMN_END - CODE_COLUMN - 5;
                        int x = CODE_COLUMN;

                        if (flags & 1) {
                                length = CODE_COLUMN - LABEL_COLUMN - 5;
                                x = LABEL_COLUMN;
                        }

                        strncpy(printable_str, str, length);
                        
                        if (strlen(str) > length) {
                                memset(printable_str + length, '.', 3);
                                if (flags & 1)
                                        printable_str[length + 3] = ':';
                                printable_str[length + 4] = ' ';
                        }

                        HIGHLIGHT_ON

                        move(i, x);
                        printw("%s%c", printable_str, (flags & 1 ? ':' : 0));

                        HIGHLIGHT_OFF
                        
                        if (flags & 1) {
                                str = print_instruction(memory, &flags);
                                memset(printable_str, 0, max_x + 1);
                                length = CODE_COLUMN_END - CODE_COLUMN - 5;

                                strncpy(printable_str, str, length);

                                if (strlen(str) > length) {
                                        memset(printable_str + length, '.', 3);
                                        if (flags & 1)
                                                printable_str[length + 3] = ':';
                                }
                                
                                HIGHLIGHT_ON

                                move(i, CODE_COLUMN);
                                printw("%s", printable_str);

                                HIGHLIGHT_OFF

                                free(str);
                        }

                        free(printable_str);
                }

                refresh();
                erase();
        }

        endwin();

        destroy_shared_memory_client();


	return 0;
}