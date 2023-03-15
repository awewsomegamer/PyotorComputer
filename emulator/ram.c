#include <ram.h>

uint8_t *memory = NULL;

void init_ram() {
        DBG(1, "Initializing RAM")

        // memory = (uint8_t *)malloc(UINT16_MAX);

        ASSERT(memory != NULL)

        DBG(1, "Initialized RAM")
}
