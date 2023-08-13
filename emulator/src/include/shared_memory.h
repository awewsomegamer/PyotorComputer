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
#define REGISTER_A_OFF 		1 // ( uint8_t  )  : Offset of the contents of the A register - MEMORY_SIZE
#define REGISTER_X_OFF 		2 // ( uint8_t  )  : Offset of the contents of the X register - MEMORY_SIZE
#define REGISTER_Y_OFF 		3 // ( uint8_t  )  : Offset of the contents of the Y register - MEMORY_SIZE
#define REGISTER_IP_OFF 	4 // ( uint16_t )  : Offset of the contents of the IP register - MEMORY_SIZE
#define REGISTER_S_OFF 		6 // ( uint8_t  )  : Offset of the contents of the S register - MEMORY_SIZE
#define REGISTER_P_OFF 		7 // ( uint8_t  )  : Offset of the contents of the P register - MEMORY_SIZE
#define PINS_OFF		8 // ( uint8_t  )  : I N R 0 0 0 0 0
				  //		     | | `--- value of pin_RES
				  //		     | `----- value of pin_NMI
				  //		     `------- value of pin_IRQ
#define EMU_FLAGS_OFF		9 // ( uint8_t  )  : W S 0 0 0 0 0 0
				  //		     | `---- Stopped
				  //		     `------ Waiting
#define IPS			10 // ( uint32_t )  : The number of instructions per second
#define BUFFER_SIZE		MEMORY_SIZE + (IPS + 4) + 2 // + 2 for the lock

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

#endif