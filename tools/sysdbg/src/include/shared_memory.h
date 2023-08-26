#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define MEMORY_SIZE 		UINT16_MAX
#define REGISTER_A_OFF 		MEMORY_SIZE + 1                   // ( uint8_t  )  : Offset of the contents of the A register - MEMORY_SIZE
#define REGISTER_X_OFF 		MEMORY_SIZE + 2                   // ( uint8_t  )  : Offset of the contents of the X register - MEMORY_SIZE
#define REGISTER_Y_OFF 		MEMORY_SIZE + 3                   // ( uint8_t  )  : Offset of the contents of the Y register - MEMORY_SIZE
#define REGISTER_PC_OFF 	MEMORY_SIZE + 4                   // ( uint16_t )  : Offset of the contents of the IP register - MEMORY_SIZE
#define REGISTER_S_OFF 		MEMORY_SIZE + 6                   // ( uint8_t  )  : Offset of the contents of the S register - MEMORY_SIZE
#define REGISTER_P_OFF 		MEMORY_SIZE + 7                   // ( uint8_t  )  : Offset of the contents of the P register - MEMORY_SIZE
#define PINS_OFF		MEMORY_SIZE + 8                   // ( uint8_t  )  : I N R 0 0 0 0 0
				                                  //		     | | `--- value of pin_RES
				                                  //		     | `----- value of pin_NMI
				                                  //		     `------- value of pin_IRQ
#define EMU_FLAGS_OFF		MEMORY_SIZE + 9                   // ( uint8_t  )  : W S R 0 0 0 0 0
                                                                  //                 | | `--- Emulator running boolean
				                                  //		   | `----- Stopped
				                                  //		   `------- Waiting
#define IPS_OFF			MEMORY_SIZE + 10                  // ( double   )  : The number of instructions per second
#define CUR_INST_OFF            MEMORY_SIZE + 18                  // ( uint16_t )  : The PC at which the current instruction starts at
#define BANK_SZ                 (0x5DC0 * 16)                    
#define BANK_A                  MEMORY_SIZE + 20                  // ( uint8_t *)  : The contents of the 16 lower half memory banks
#define BANK_B                  MEMORY_SIZE + 20 + BANK_SZ        // ( uint8_t *)  : The contents of the 16 lower half memory banks
#define BUFFER_SIZE		MEMORY_SIZE + BANK_SZ + 1         // + 1 for the lock

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

extern uint8_t *memory;
extern uint8_t lock_owned;

void init_shared_memory_host();
void init_shared_memory_client();
void destroy_shared_memory();

void shared_memory_acquire_lock();
void shared_memory_release_lock();

#endif