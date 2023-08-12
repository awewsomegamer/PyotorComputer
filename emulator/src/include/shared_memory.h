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

#define MEMORY_SIZE 		UINT16_MAX
#define REGISTER_A_OFF 		0 // ( uint8_t  )  : Offset of the contents of the A register - MEMORY_SIZE
#define REGISTER_X_OFF 		1 // ( uint8_t  )  : Offset of the contents of the X register - MEMORY_SIZE
#define REGISTER_Y_OFF 		2 // ( uint8_t  )  : Offset of the contents of the Y register - MEMORY_SIZE
#define REGISTER_IP_OFF 	3 // ( uint16_t )  : Offset of the contents of the IP register - MEMORY_SIZE
#define REGISTER_S_OFF 		5 // ( uint8_t  )  : Offset of the contents of the S register - MEMORY_SIZE
#define REGISTER_P_OFF 		6 // ( uint8_t  )  : Offset of the contents of the P register - MEMORY_SIZE
#define PINS_OFF		7 // ( uint8_t  )  : I N R 0 0 0 0 0
				  //		     | | `--- value of pin_RES
				  //		     | `----- value of pin_NMI
				  //		     `------- value of pin_IRQ
#define EMU_FLAGS_OFF		8 // ( uint8_t  )  : W S 0 0 0 0 0 0
				  //		     | `---- Stopped
				  //		     `------ Waiting
#define IPS			9 // ( uint32_t )  : The number of instructions per second
#define BUFFER_SIZE		MEMORY_SIZE + (IPS + 4) + 2 // + 2 for the lock

extern caddr_t memory;

void init_shared_memory();

#endif