#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "shared_memory.h"

#ifdef DEBUG
        #define DBG(msg, what) \
                if (msg == 1) { \
                        printf("[%s@%d] : ", __FILE__, __LINE__); \
                        what \
                        printf("\n"); \
                } else { \
                        what; \
                }
#else
        #define DBG(msg, what) ;
#endif

#define ASSERT(cond) if (!(cond)) { printf("[%s@%d] : Assertion "#cond" failed, exiting\n", __FILE__, __LINE__); exit(1); }

extern uint8_t running;
extern uint64_t *sys_ips;

#endif