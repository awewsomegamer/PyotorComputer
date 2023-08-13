#include "include/ram.h"
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/video.h"
#include "include/cpu/cpu.h"
#include <stdio.h>

uint8_t mem_byte_read(uint16_t address) {
        return *(memory + address);
}

void mem_byte_write(uint8_t byte, uint16_t address) {
        if (KERNAL_MEM(address)) {
                DBG(1, printf("Tried to write to KERNAL ROM address");)
                
                char *memdump = malloc(48);

                for (int i = 0; i < 16; i++)
                        sprintf(memdump + i * 3, "%02X ", *(memory + i));
                DBG(1, printf("%s", memdump);)

                DBG(1, reg_dump_65C02();)
                
                return;
        }

        *(memory + address) = byte;
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
