#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <SDL2/SDL_stdinc.h>
#define MAX_EVENTS 128

#include "global.h"

struct scheduled_function {
	void (*function)();
	uint64_t when;
};

void schedule_function(void (*function)(), uint64_t when); // When = 64 bit SDL tick
void do_task();

#endif