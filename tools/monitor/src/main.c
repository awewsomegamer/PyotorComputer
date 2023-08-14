#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global.h"
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

        max_y--;
        max_x--;

        if (max_x == 0 || max_y == 0) {
                // Screen too small
                exit(1);
        }
}

int main(int argc, char **argv) {
        FILE *code;
        FILE *labels;

        if (argc > 1 && strcmp(argv[1], "-d") == 0) {
                if (argc < 2) {
                        printf("Disassembly mode:\nmonitor -d assembled.bin [labels.txt]\n");
                        return 1;
                }

                code = fopen(argv[2], "r");

                if (argc > 3) {
                        labels = fopen(argv[3], "r");
                        parse_labels(labels);
                        fclose(labels);
                }

                assert(code != NULL);

                fseek(code, 0, SEEK_END);
                size_t size = ftell(code);
                fseek(code, 0, SEEK_SET);
                
                uint8_t *buffer = malloc(size);
                fread(buffer, 1, size, code);

                while (pc < size) {
                        char *str = print_instruction(buffer);
                        // printf("%s\n", str);
                        free(str);
                }

                return 0;
        }

        if (argc > 1) {
                labels = fopen(argv[1], "r");

        }

        switch (argc) {
        case 2: {  

                break;
        }

        case 3: {
                
                break;
        }

        case 4: {

                break;
        }

        case 5: {

                break;
        }

        default: {
                init_shared_memory_client();
                init_ncurses();

                // While running
                while ((*(memory + EMU_FLAGS_OFF) >> 5) & 1) {
                        

                }

	        destroy_shared_memory_client();

                break;
        }
        }

	return 0;
}