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

#define INFO_COLUMN REGISTERS_COLUMN
#define INFO_END_COLUMN (max_x - 1)
#define INFO_START_ROW (REGISTERS_END_ROW)
#define INFO_END_ROW ((max_y / 4) + INFO_START_ROW)
#define INFO_LABEL " INFORMATION "

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

#define DEBUG_MODE_NONE         0
#define DEBUG_MODE_BREAKPOINT   1
#define DEBUG_MODE_STEP         2
#define DEBUG_MODE_CMD          3

#define DEBUG_ACTION_A          0
#define DEBUG_ACTION_B          1
#define DEBUG_ACTION_C          2

static const char *debugger_mode_strings[] = {
        [DEBUG_MODE_NONE] = "None",
        [DEBUG_MODE_BREAKPOINT] = "Breakpoint",
        [DEBUG_MODE_STEP] = "Step",
        [DEBUG_MODE_CMD] = "Command",
};

#define DEBUG_MODE_TOGGLE(mode, on, off) \
        debugger_mode = (debugger_mode == mode) ? DEBUG_MODE_NONE : mode; \
        if (debugger_mode == mode) \
                on                 \
        else                       \
                off

extern int max_x;
extern int max_y;
extern uint8_t debugger_mode;
extern uint8_t running;

size_t char_hash(char c);

#endif