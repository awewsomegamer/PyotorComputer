#ifndef RAM_H
#define RAM_H

#include <global.h>

#define PAGE_SIZE 0x100
#define KB_16 0x3E80

enum memory_map {
        ZERO_PAGE_BASE = 0x0000,
        STACK_BASE = 0x0100,
        VIDEO_MEM_BASE = 0x0200,
        PROGRAM_MEM_BASE = VIDEO_MEM_BASE + KB_16,
        KERNEL_MEM_BASE = PROGRAM_MEM_BASE + KB_16 // Not final
};

#define ZERO_PAGE(a)   (ZERO_PAGE_BASE   <= a && a <= ZERO_PAGE_BASE + PAGE_SIZE - 1)
#define STACK(a)       (STACK_BASE       <= a && a <= STACK_BASE + PAGE_SIZE - 1)
#define VIDEO_MEM(a)   (VIDEO_MEM_BASE   <= a && a <= PROGRAM_MEM_BASE - 1)
#define PROGRAM_MEM(a) (PROGRAM_MEM_BASE <= a && a <= KERNEL_MEM_BASE - 1)

uint8_t mem_byte_read(uint16_t);
void mem_byte_write(uint8_t, uint16_t);

void load_file(uint16_t, FILE *, char *);

void init_ram();

#endif