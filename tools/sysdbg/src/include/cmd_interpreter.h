#ifndef CMD_INTERPRETER_H
#define CMD_INTERPRETER_H

#include "global.h"

#define CMD_BREAK_HASH (uint64_t)0xA5B1E0

void execute_cmd();
void cmd_receive_char(char c);

#endif