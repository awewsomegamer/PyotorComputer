#ifndef RAM_H
#define RAM_H

#include <global.h>

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
                0x?? - 0x?? : User

                Detailed:
                0x00 : Reserved
                0x01 : Memory Map Mask
                0x03 : Keyboard Scancode
                0x04 : Reserved

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

extern uint8_t *general_memory;

uint8_t mem_byte_read(uint16_t address);
void mem_byte_write(uint8_t byte, uint16_t address);

void load_file(uint16_t address, char *name);

void init_ram();

#endif
