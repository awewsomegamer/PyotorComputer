#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
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
#define REGISTER_A_OFF 		MEMORY_SIZE + 1         // ( uint8_t  )  : Offset of the contents of the A register - MEMORY_SIZE
#define REGISTER_X_OFF 		MEMORY_SIZE + 2         // ( uint8_t  )  : Offset of the contents of the X register - MEMORY_SIZE
#define REGISTER_Y_OFF 		MEMORY_SIZE + 3         // ( uint8_t  )  : Offset of the contents of the Y register - MEMORY_SIZE
#define REGISTER_IP_OFF 	MEMORY_SIZE + 4         // ( uint16_t )  : Offset of the contents of the IP register - MEMORY_SIZE
#define REGISTER_S_OFF 		MEMORY_SIZE + 6         // ( uint8_t  )  : Offset of the contents of the S register - MEMORY_SIZE
#define REGISTER_P_OFF 		MEMORY_SIZE + 7         // ( uint8_t  )  : Offset of the contents of the P register - MEMORY_SIZE
#define PINS_OFF		MEMORY_SIZE + 8         // ( uint8_t  )  : I N R 0 0 0 0 0
				                        //		     | | `--- value of pin_RES
				                        //		     | `----- value of pin_NMI
				                        //		     `------- value of pin_IRQ
#define EMU_FLAGS_OFF		MEMORY_SIZE + 9         // ( uint8_t  )  : W S R 0 0 0 0 0
                                                        //                 | | `-- Emulator running boolean
				                        //		     | `---- Stopped
				                        //		     `------ Waiting
#define IPS			MEMORY_SIZE + 10        // ( double  )  : The number of instructions per second
#define BUFFER_SIZE		MEMORY_SIZE + (IPS + 8) + 1 // + 1 for the lock

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

void init_shared_memory_host();
void destroy_shared_memory_host();

void init_shared_memory_client();
void destroy_shared_memory_client();

void shared_memory_acquire_lock();
void shared_memory_release_lock();

#endif