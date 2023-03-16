#include <cpu.h>
#include <ram.h>

// Initial states of registers
uint8_t io_port = 0;
uint8_t register_a = 0;
uint8_t register_x = 0;
uint8_t register_y = 0;
uint8_t register_s = 0xFF;
struct {
        uint8_t C : 1;
        uint8_t Z : 1;
        uint8_t I : 1;
        uint8_t D : 1;
        uint8_t B : 1;
        uint8_t unused : 1;
        uint8_t V : 1;
        uint8_t N : 1;
} register_p;
uint16_t pc = 0;

void (*instruction[0xFF])();

// Preform one cycle on the 6502
void tick_6502() {
        (*instruction[mem_byte_read(pc++)])();
}

void INST_LDA_IMM() { register_a = mem_byte_read(pc++); }
void INST_LDX_IMM() { register_x = mem_byte_read(pc++); }
void INST_LDY_IMM() { register_y = mem_byte_read(pc++); }

void INST_STA_IMM() { mem_byte_write(register_a, mem_byte_read(pc++) | (mem_byte_read(pc++) << 8)); }
void INST_STX_IMM() { mem_byte_write(register_a, mem_byte_read(pc++) | (mem_byte_read(pc++) << 8)); }
void INST_STY_IMM() { mem_byte_write(register_a, mem_byte_read(pc++) | (mem_byte_read(pc++) << 8)); }

void reg_dump_6502() {
        printf("-- 6502 REG DUMP --\n");
        printf("A  : %02X (%d)\n", register_a, register_a);
        printf("X  : %02X (%d)\n", register_x, register_x);
        printf("Y  : %02X (%d)\n", register_y, register_y);
        printf("S  : %02X (%d)\n", register_s, register_s);
        printf("P  : %02X (%d)\n", register_p, register_p);
        printf("PC : %04X (%d)\n", pc, pc);
        printf("-- 6502 REG DUMP END --\n");
}

// Initialize the 6502
void init_6502() {
        DBG(1, "Initializing 6502 CPU")

        instruction[0xA9] = INST_LDA_IMM;
        instruction[0xA2] = INST_LDX_IMM;
        instruction[0xA0] = INST_LDY_IMM;
        
        instruction[0x8D] = INST_STA_IMM;
        instruction[0x8E] = INST_STX_IMM;
        instruction[0x8C] = INST_STY_IMM;

        DBG(1, "Initialized 6502 CPU")
}