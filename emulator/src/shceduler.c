#include "include/global.h"
#include "include/scheduler.h"
#include <SDL2/SDL_timer.h>
#include <string.h>

uint8_t schedule_bmp[MAX_EVENTS / 8];
struct scheduled_function scheduled[MAX_EVENTS];

// Insert a function to be done at a specific time
void schedule_function(void (*function)(), uint64_t when) {
	for (int i = 0; i < MAX_EVENTS / 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (((schedule_bmp[i] >> j) & 1) == 0) {
				schedule_bmp[i] |= 1 << j;
				int index = i * 8 + j;
				scheduled[index].function = function;
				scheduled[index].when = when;
				DBG(1, printf("Scheduled task %d with pointer of %p to execute at %d ticks", index, function, when);)

				return;
			}
		}
	}
}

// Remove the first task
void task_complete(int index) {
	int byte = index / 8;
	int bit = index % 8;
	schedule_bmp[byte] &= ~(1 << bit);
	memset(&scheduled[index], 0, sizeof(struct scheduled_function));

	DBG(1, printf("Marked task %d as completed", index);)
}

// Do the first task available to do and remove it from the stack
void do_task() {
	for (int i = 0; i < MAX_EVENTS; i++) {
		if (scheduled[i].when < SDL_GetTicks64() && scheduled[i].when != 0) {
			DBG(1, printf("Doing task %d", i);)
			(*scheduled[i].function)();
			task_complete(i);
		}
	}
}
