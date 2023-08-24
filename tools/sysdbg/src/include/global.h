#ifndef GLOBAL_H
#define GLOBAL_H

#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include <curses.h>

#define LABEL_COLUMN 0
#define CODE_COLUMN (max_x / 4)
#define CODE_COLUMN_END (max_x / 2)
#define DISASM_START_ROW 0 //((max_y / 2) - 1)
#define DISASM_END_ROW max_y
#define DISASM_LABEL " LIVE DISASSEMBLY "

#define REGISTERS_COLUMN (CODE_COLUMN_END + 1)
#define REGISTERS_END_COLUMN (max_x - 1)
#define REGISTERS_START_ROW 0
#define REGISTERS_END_ROW (max_y / 2)
#define REGISTERS_LABEL " REGISTERS "

#define SYS_INFO_COLUMN REGISTERS_COLUMN
#define SYS_INFO_END_COLUMN (max_x - 1)
#define SYS_INFO_START_ROW (REGISTERS_END_ROW)
#define SYS_INFO_END_ROW ((max_y / 4) + SYS_INFO_START_ROW)
#define SYS_INFO_LABEL " SYSTEM INFORMATION "

#define SYSDBG_COLOR_REGULAR 0
#define SYSDBG_COLOR_HIGHLIGHTED 1
#define SYSDBG_COLOR_GREEN 2
#define SYSDBG_COLOR_RED 3

#define HIGHLIGHT_ON if ((has_colors() == TRUE)) \
                                attron(COLOR_PAIR(SYSDBG_COLOR_HIGHLIGHTED));
#define HIGHLIGHT_OFF if ((has_colors() == TRUE)) \
                                attroff(COLOR_PAIR(SYSDBG_COLOR_HIGHLIGHTED));
#define GREEN_ON if ((has_colors() == TRUE)) \
                                attron(COLOR_PAIR(SYSDBG_COLOR_GREEN));
#define GREEN_OFF if ((has_colors() == TRUE)) \
                                attroff(COLOR_PAIR(SYSDBG_COLOR_GREEN));
#define RED_ON if ((has_colors() == TRUE)) \
                                attron(COLOR_PAIR(SYSDBG_COLOR_RED));
#define RED_OFF if ((has_colors() == TRUE)) \
                                attroff(COLOR_PAIR(SYSDBG_COLOR_RED));

extern int max_x;
extern int max_y;
extern uint8_t running;

#endif