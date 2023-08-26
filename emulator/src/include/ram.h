#ifndef RAM_H
#define RAM_H

#include "global.h"

#define PAGE_SIZE 0x100
#define KB_16 0x3E80

/*
        Big Memory Map:
        0x0000 - 0x00FF : Zero Page
        0x0100 - 0x01FF : Stack
        0x0200 - 0xBD7F : Program Memory
        0xBD80 - 0xDCBF : Kernal RAM
        0xDCC0 - 0xFFFF : Kernal ROM
        
        Zero Page Map:
                Big Idea:
                0x00 - 0x?? : Kernal
                0x?? - 0xFF : User

                Detailed:
                0x00 : Bank teller *
                0x01 : Reserved for Kernal (Temporary value #2)
                0x02 : Memory Map Mask
                0x03 : Keyboard Scancode
                0x04 : Reserved for Kernal (Temporary value #1)

                * Low nibble contains the memory bank index for
                  the first half of program memory. The high 
                  nibble contains the memory bank index for the
                  second half of program memory.

                  This would allow for 768 KB supposing the program
                  memory is 48 KB.
*/

enum memory_map {
        ZERO_PAGE_BASE = 0x0000,
        STACK_BASE = 0x0100,
        PROGRAM_MEM_BASE = 0x0200,
        KERNAL_DAT_BASE = PROGRAM_MEM_BASE + (KB_16 * 3),
        KERNAL_MEM_BASE = KERNAL_DAT_BASE + (KB_16 / 2),
        MEM_TOP = UINT16_MAX
};

#define ZERO_PAGE(a)       (ZERO_PAGE_BASE   <= a && a <= ZERO_PAGE_BASE + PAGE_SIZE - 1)
#define STACK(a)           (STACK_BASE       <= a && a <= STACK_BASE + PAGE_SIZE - 1)
#define PROGRAM_MEM(a)     (PROGRAM_MEM_BASE <= a && a <= KERNAL_DAT_BASE - 1)
#define KERNAL_DAT_MEM(a)  (KERNAL_DAT_BASE  <= a && a <= KERNAL_MEM_BASE - 1)
#define KERNAL_MEM(a)      (KERNAL_MEM_BASE  <= a && a <= MEM_TOP - 1)

#define ZERO_PAGE_SZ            256
#define STACK_SZ                256
#define PROGRAM_MEM_SZ          (KB_16 * 3)
#define KERNAL_MEM_SZ           (KB_16 / 2)
#define KERNAL_ROM_SZ           (KB_16 / 2)

uint8_t mem_byte_read(uint16_t address);
void mem_byte_write(uint8_t byte, uint16_t address);

void load_file(uint16_t address, char *name);

void init_system_memory();

#endif
