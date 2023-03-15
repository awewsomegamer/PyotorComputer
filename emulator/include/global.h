#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef DEBUG
        #define DBG(msg, what) \
                if (msg == 1) { \
                        printf("[%s@%d] : "what"\n", __FILE__, __LINE__); \
                } else { \
                        what; \
                }
#else
        #define DBG(msg, what) ;
#endif

#define ASSERT(cond) if (!(cond)) { printf("[%s@%d] : Assertion "#cond" failed, exiting\n", __FILE__, __LINE__); exit(1); }

#endif