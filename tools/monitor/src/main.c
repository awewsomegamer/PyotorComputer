#include <stdio.h>
#include <stdlib.h>
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/disassemble.h"

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
        switch (argc) {
        case 2: {
                FILE *code = fopen(argv[1], "r");
                
                fseek(code, 0, SEEK_END);
                size_t size = ftell(code);
                fseek(code, 0, SEEK_SET);

                uint8_t *buffer = malloc(size);
                fread(buffer, 1, size, code);

                for (int i = 0; i < 10; i++) {
                        char *str = print_instruction(buffer);
                        printf("%s\n", str);
                        free(str);
                }


                break;
        }

        case 3: {

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