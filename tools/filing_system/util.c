#include "main.h"
#include <stdio.h>

size_t allocated_thingimabobs = 0;

void write_to_fs(void *data, long where) {
	fseek(fs_file, where, SEEK_SET);
	fwrite(data, 1, COMMON_STRUCT_SIZE, fs_file);
	fclose(fs_file);
	fs_file = fopen(fs_name, "r+");
}

void read_from_fs(void *data, long where) {
	fseek(fs_file, where, SEEK_SET);
	fread(data, 1, COMMON_STRUCT_SIZE, fs_file);
}

char get_char() {
	char choice;
	for (;;)
		if ((choice = getchar()) && (choice >= 0x20 && choice <= 0x7E))
			break;
	
	return choice;
}

void memcset(void *buffer, uint8_t value, uint8_t condition, size_t length) {
	for (size_t i = 0; i < length; i++)
		if (*((uint8_t *)buffer + i) == condition)
			*((uint8_t *)buffer + i) = value;
}

// 0 - 65 : File index in initial directory list
// 66 - 131 : File index in current directory list
// 132 : File not found
uint8_t find_file(char *name) {
	for (int i = 0; i < 62; i++)
		if (strcmp(name, init->entries[i].name) == 0)
			return i;

	if (init->next_directory_listing == 0)
		return 132;

	current_dir_list_sector = init->next_directory_listing;

	while (current_dir_list_sector != 0) {
		fseek(fs_file, current_dir_list_sector * SECTOR_SIZE, SEEK_SET);
		fread(current_dir_list, 1, COMMON_STRUCT_SIZE, fs_file);

		for (int i = 0; i < 62; i++)
			if (strcmp(name, current_dir_list->entries[i].name) == 0)
				return i + 66;

		current_dir_list_sector = current_dir_list->next_directory_listing;
	}

	return 132;
}