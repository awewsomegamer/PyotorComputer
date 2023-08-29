#include "include/input.h"
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/cmd_interpreter.h"

void perform_action(int type) {
	// Type C actions, triggered by backspace
	if (type == DEBUG_ACTION_C) {
		switch (debugger_mode) {
		case DEBUG_MODE_BREAKPOINT: {
			// Go back one instruction
			// Figure this out somehow
			return;
		}
		}
	}

	// Type B actions, triggered by enter
	if (type == DEBUG_ACTION_B) {
		switch (debugger_mode) {
		case DEBUG_MODE_BREAKPOINT: {
			// Go forward one instruction
			return;
		}

		case DEBUG_MODE_CMD: {
			DEBUG_MODE_TOGGLE(DEBUG_MODE_CMD, ;, ;)
			execute_cmd();

			break;
		}
		}	
	}

	// Type A actions, triggered by space
	switch (debugger_mode) {
	case DEBUG_MODE_BREAKPOINT: {
		// Insert breakpoint

		return;
	}

	case DEBUG_MODE_STEP: {
		// Step forwards once
		return;
	}
	}
}

void interpret_key(int key) {
	uint64_t *ips = (uint64_t *)(memory + IPS_OFF);

	switch (key) {
	case ',': { // Basic speed control
		*ips = 0;
		break;
	}

	case '.': { // Basic speed control
		*ips += 1;
		break;
	}

	case '\'': { // Basic speed control
		*ips = 3500000;
		break;
	}

	case 'b': { // Toggle break point mode
		DEBUG_MODE_TOGGLE(DEBUG_MODE_BREAKPOINT,
			*ips = 0; // Allow user to adjust diassembly and set breakpoint
		,
			;
		)

		break;
	}

	case 's': { // Toggle manual step mode
		DEBUG_MODE_TOGGLE(DEBUG_MODE_STEP, ;, ;)

		break;
	}

	case 'c': {
		DEBUG_MODE_TOGGLE(DEBUG_MODE_CMD, ;, ;)

		break;
	}

	case ' ': { // "Action" key
		perform_action(DEBUG_ACTION_A);

		break;
	}

	case '\n': { // "Action" key
		perform_action(DEBUG_ACTION_B);

		break;
	}

	case '\b': { // "Action" key
		perform_action(DEBUG_ACTION_C);

		break;
	}

	default: {
		if (debugger_mode == DEBUG_MODE_CMD) {
			cmd_receive_char(key);

			break;
		}
	}
	}
}