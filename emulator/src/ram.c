#include "include/ram.h"
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/video.h"
#include "include/cpu/cpu.h"
#include <stdio.h>
#include <string.h>

uint8_t **bank_a = NULL;
uint8_t **bank_b = NULL;

uint8_t mem_byte_read(uint16_t address) {
        shared_memory_acquire_lock();
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

        if (address == 0x0000) {
                DBG(1, printf("Switching to Bank A #%d and to Bank B #%d", byte & 0b1111, (byte >> 4) & 0b1111);)

                memcpy(bank_a[*memory & 0b1111], memory + PROGRAM_MEM_BASE, PROGRAM_MEM_SZ / 2);
                memcpy(bank_b[(*memory >> 4) & 0b1111], memory + PROGRAM_MEM_BASE + (PROGRAM_MEM_SZ / 2), PROGRAM_MEM_SZ / 2);

                memcpy(memory + PROGRAM_MEM_BASE, bank_a[byte & 0b1111], PROGRAM_MEM_SZ / 2);
                memcpy(memory + PROGRAM_MEM_BASE + (PROGRAM_MEM_SZ / 2), bank_b[(byte >> 4) & 0b1111], PROGRAM_MEM_SZ / 2);
        }

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

        bank_a = (uint8_t **)malloc(sizeof(uint8_t *) * 16);
        bank_b = (uint8_t **)malloc(sizeof(uint8_t *) * 16);

        ASSERT(bank_a != NULL);
        ASSERT(bank_b != NULL);

        for (int i = 0; i < 16; i++) {
                bank_a[i] = (uint8_t *)malloc(PROGRAM_MEM_SZ / 2);
                bank_b[i] = (uint8_t *)malloc(PROGRAM_MEM_SZ / 2);

                ASSERT(bank_a[i] != NULL);
                ASSERT(bank_b[i] != NULL);

                DBG(1, printf("Bank A & B #%d allocated", i);)

                memset(bank_a[i], 0, PROGRAM_MEM_SZ / 2);
                memset(bank_b[i], 0, PROGRAM_MEM_SZ / 2);

                DBG(1, printf("Bank A & B #%d zeroed", i);)
        }
}

void destroy_system_memory() {
        DBG(1, printf("Destroying memory banks");)

        for (int i = 0; i < 16; i++) {
                free(bank_a[i]);
                free(bank_b[i]);

                DBG(1, printf("Destroyed bank A & B #%d", i);)
        }

        free(bank_a);
        free(bank_b);

        DBG(1, printf("Destroyed memory banks");)
}