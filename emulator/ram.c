#include <ram.h>
#include <cga.h>
#include <cpu.h>

uint8_t *memory = NULL;

uint8_t mem_byte_read(uint16_t address) {
        if ((*(memory + 1) & IO_VIDEO_MASK) && VIDEO_MEM(address)) {
                return cga_mem_read(address);
        }

        return *(memory + address);
}

void mem_byte_write(uint8_t byte, uint16_t address) {
        if ((*(memory + 1) & IO_VIDEO_MASK) && VIDEO_MEM(address)) {
                cga_mem_write(address, byte);
                return;
        }

        if (KERNAL_MEM(address)) {
                DBG(1, printf("Tried to write to KERNAL ROM address");)
                return;
        }

        *(memory + address) = byte;
}

void load_file(uint16_t address, FILE *file, char* name) {
        DBG(1, printf("Loading file \"%s\" at 0x%04X", name, address);)
        fseek(file, 0, SEEK_END);
        size_t file_length = ftell(file);
        fseek(file, 0, SEEK_SET);
        ASSERT(file_length < UINT16_MAX);
        
        fread(memory + address, 1, file_length, file);
        DBG(1, printf("Loaded file \"%s\" at 0x%04X", name, address);)
}

void init_ram() {
        DBG(1, printf("Initializing RAM");)

        memory = (uint8_t *)malloc(UINT16_MAX);

        ASSERT(memory != NULL)

        DBG(1, printf("Initialized RAM");)
}
