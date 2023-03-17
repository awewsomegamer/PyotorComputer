#include <ram.h>
#include <cpu.h>

uint8_t *memory = NULL;

uint8_t mem_byte_read(uint16_t address) {
        if (io_port & IO_VIDEO_MASK && VIDEO_MEM(address)) {
                // Memory 0x0200 -> 0x4080 is video memory
        }

        return *(memory + address);
}

void mem_byte_write(uint8_t byte, uint16_t address) {
        if (io_port & IO_VIDEO_MASK && VIDEO_MEM(address)) {
                // Memory 0x0200 -> 0x4080 is video memory
        }

        *(memory + address) = byte;
}

void load_file(uint16_t address, FILE *file) {
        fseek(file, 0, SEEK_END);
        size_t file_length = ftell(file);
        fseek(file, 0, SEEK_SET);
        ASSERT(file_length < UINT16_MAX);
        
        fread(memory + address, 1, file_length, file);
}

void init_ram() {
        DBG(1, printf("Initializing RAM");)

        memory = (uint8_t *)malloc(UINT16_MAX);

        ASSERT(memory != NULL)

        DBG(1, printf("Initialized RAM");)
}
