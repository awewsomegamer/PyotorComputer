#include "include/input.h"
#include "include/disassemble.h"
#include "include/global.h"
#include "include/shared_memory.h"
#include "include/cmd_interpreter.h"

#define IPS *(uint64_t *)(memory + IPS_OFF)

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

	default: {
		if ((disasm_flags >> DISASM_FLAG_BREAK) & 1) {
			disasm_flags &= ~(1 << DISASM_FLAG_BREAK);
			IPS = 3500000;
		}
	}
	}
}

void interpret_key(int key) {
	// Action switch
	switch (key) {
	case ' ': { // "Action" key
		if (debugger_mode != DEBUG_MODE_CMD)
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
	}

	if (debugger_mode == DEBUG_MODE_CMD) {
		cmd_receive_char(key);

		return;
	}

	// Mode / Key switch
	switch (key) {
	case ',': { // Basic speed control	
		disasm_flags &= ~(1 << DISASM_FLAG_BREAK);

		IPS = 0;

		break;
	}

	case '.': { // Basic speed control
		disasm_flags &= ~(1 << DISASM_FLAG_BREAK);

		IPS += 1;

		break;
	}

	case '\'': { // Basic speed control
		disasm_flags &= ~(1 << DISASM_FLAG_BREAK);

		IPS = 3500000;

		break;
	}

	case 'b': { // Toggle break point mode
		DEBUG_MODE_TOGGLE(DEBUG_MODE_BREAKPOINT,
			IPS = 0; // Allow user to adjust diassembly and set breakpoint
		,
			IPS = 3500000;
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
	}
}