#include <global.h>
#include <cpu.h>
#include <ram.h>

int main(int argc, char **argv) {
        init_6502();
        init_ram();

        return 0;
}