#include <stdio.h>
#include <stdlib.h>
#include "include/shared_memory.h"

int main() {
	init_shared_memory_client();

	// While running
	while ((*(memory + EMU_FLAGS_OFF) >> 5) & 1) {
		for (int i = 0; i < 16; i++)
			printf("%02X ", *(memory + i));
		printf("\r");
	}

	return 0;
}