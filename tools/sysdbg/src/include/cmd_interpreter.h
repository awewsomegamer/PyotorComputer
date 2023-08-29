#ifndef CMD_INTERPRETER_H
#define CMD_INTERPRETER_H

#include "global.h"

#define CMD_BREAK_HASH 0x6085C14
#define CMD_CLEAR_HASH 0x60DB16B // Unimplemented

extern int command_buffer_idx;
extern char command_buffer[512];
extern char command_response[512];

void execute_cmd();
void cmd_receive_char(char c);

#endif