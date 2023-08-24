#include "include/input.h"
#include "include/global.h"
#include "include/shared_memory.h"

void interpret_key(int key) {
	uint64_t *ips = (uint64_t *)(memory + IPS_OFF);

	switch (key) {
	case 'c': {
		running = 0;
		break;
	}
	case ',': {
		*ips = 0;
		break;
	}
	case '.': {
		*ips += 1;
		break;
	}
	case '\'': {
		*ips = 3500000;
		break;
	}
	}
}