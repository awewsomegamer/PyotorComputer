#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ASSERT(cond) \
        if (!(cond)) { \
                printf("[%s@%d] : Assertion: "#cond" failed\n", __FILE__, __LINE__); \
                exit(1); \
        }


extern FILE *_input_file;
extern FILE *_output_file;

#endif