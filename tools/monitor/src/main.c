#include <stdio.h>
#include <stdlib.h>
#include "include/shared_memory.h"

#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include <curses.h>

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

int main() {
	init_shared_memory_client();
	init_ncurses();

	// While running
	while ((*(memory + EMU_FLAGS_OFF) >> 5) & 1) {
		

	}

	destroy_shared_memory_client();

	return 0;
}