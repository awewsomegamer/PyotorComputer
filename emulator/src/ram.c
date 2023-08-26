#include "include/ram.h"
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/video.h"
#include "include/cpu/cpu.h"
#include <stdio.h>
#include <string.h>

uint8_t *bank_a = NULL;
uint8_t *bank_b = NULL;

uint8_t mem_byte_read(uint16_t address) {
        shared_memory_acquire_lock();

        if (PROGRAM_MEM(address)) {
                uint16_t offset = (address - PROGRAM_MEM_BASE);
                uint8_t bank_bucket = offset > PROGRAM_MEM_SZ / 2 ? 1 : 0;
                int base = PROGRAM_MEM_SZ * (bank_bucket ? ((*memory >> 4) & 0b1111) : 
                                                           ((*memory)      & 0b1111));

                uint8_t value = *((bank_bucket ? bank_b : bank_a) + (offset % PROGRAM_MEM_SZ / 2) + base);

                shared_memory_release_lock();
                return value;
        }


        uint8_t value = *(memory + address);
        shared_memory_release_lock();

        return value;
}

void mem_byte_write(uint8_t byte, uint16_t address) {
        shared_memory_acquire_lock();

        if (KERNAL_MEM(address)) {
                DBG(1, printf("Tried to write to KERNAL ROM address %04X", address);)
                
                char *memdump = malloc(48);

                for (int i = 0; i < 16; i++)
                        sprintf(memdump + i * 3, "%02X ", *(memory + i));
                DBG(1, printf("%s", memdump);)

                DBG(1, reg_dump_65C02();)
                
                return;
        }

        if (PROGRAM_MEM(address)) {
                uint16_t offset = (address - PROGRAM_MEM_BASE);
                uint8_t bank_bucket = offset > PROGRAM_MEM_SZ / 2 ? 1 : 0;
                int base = PROGRAM_MEM_SZ * (bank_bucket ? ((*memory >> 4) & 0b1111) : 
                                                           ((*memory)      & 0b1111));

                *((bank_bucket ? bank_b : bank_a) + (offset % PROGRAM_MEM_SZ / 2) + base) = byte;

                shared_memory_release_lock();
                return;
        }

        if (address == 0x0000)
                DBG(1, printf("Switching to Bank A #%d and to Bank B #%d", byte & 0b1111, (byte >> 4) & 0b1111);)

        *(memory + address) = byte;
        
        shared_memory_release_lock();
}

void load_file(uint16_t address, char *name) {
        FILE *file = fopen(name, "r");

        ASSERT(file != NULL)

        DBG(1, printf("Loading file \"%s\" at 0x%04X", name, address);)
        fseek(file, 0, SEEK_END);
        size_t file_length = ftell(file);
        fseek(file, 0, SEEK_SET);
        ASSERT(file_length < UINT16_MAX);
        
        fread(memory + address, 1, file_length, file);
        DBG(1, printf("Loaded file \"%s\" at 0x%04X", name, address);)

        fclose(file);
}

void init_system_memory() {
        DBG(1, printf("Initializing memory banks");)

        bank_a = (uint8_t *)(memory + BANK_A_OFF);
        bank_b = (uint8_t *)(memory + BANK_B_OFF);

        ASSERT(bank_a != NULL);
        ASSERT(bank_b != NULL);
}
