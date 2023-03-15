#ifndef ISA_H
#define ISA_H

#include <global.h>

struct instruction {
        uint8_t opcode;
        uint8_t supported_operands;
};

struct reg {
        char *name;      
};

void init_isa();

#endif