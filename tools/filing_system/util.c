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

uint8_t *get_file_data(uint8_t file_index, size_t *size) {
	if (file_index == FILE_NOT_FOUND) {
		printf("File not found\n");
		return NULL;
	}

	uint8_t *buffer = NULL;

	if (file_index < 66) {
		struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);
		read_from_fs(file_init, init->entries[file_index].sector * SECTOR_SIZE);

		*size = (file_init->size_in_sectors / 2) * 1000 - file_init->bytes_unused_last_kb;
		
		if (*size == 0)
			return NULL;

		buffer = malloc((file_init->size_in_sectors / 2) * 1000);
		memcpy(buffer, file_init->data, 1000);

		uint16_t next_sector = file_init->next_descriptor;

		free(file_init);

		if (next_sector == 0)
			return buffer;
		
		struct file_descriptor *desc = (struct file_descriptor *) malloc(COMMON_STRUCT_SIZE);
		size_t offset = 1000;
		size_t d_size = *size - 1000;
		while (next_sector != 0) {
			read_from_fs(desc, next_sector * SECTOR_SIZE);
			memcpy(buffer + offset, desc->data, 1000);

			offset += 1000;
			next_sector = desc->next_descriptor;
		}

		free(desc);
		
		return buffer;
	}

	return NULL;
}

void write_file_data(FILE *file, size_t file_size, uint8_t file_index, uint8_t overwrite_existing) {
	struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);
	memset(file_init, 0, COMMON_STRUCT_SIZE);
	
	uint16_t size_in_sectors = ((file_size + 1000) / 1000) == 1 ? 2 : (((file_size + 1000) / 1000) * 2);
	uint16_t bytes_unused_last_kb = ((size_in_sectors / 2) * 1000) - file_size;
	uint16_t cur_sector = (file_index < 66) ? init->entries[file_index].sector : current_dir_list->entries[file_index - 66].sector;

	if (overwrite_existing)
		read_from_fs(file_init, cur_sector * SECTOR_SIZE);

	fread(file_init->data, 1, (file_size < 1000 ? file_size : 1000), file);

	if (file_size > 1000) {
		struct file_descriptor *file_desc = (struct file_descriptor *) malloc(COMMON_STRUCT_SIZE);
		uint16_t sector = init->next_free_sector;
		uint16_t next_descriptor = file_init->next_descriptor;
		size_t current_byte = 1000;

		if (!overwrite_existing) {
			file_init->next_descriptor = sector;

			backtrack:

			init->next_free_sector += 2;

			for (; current_byte < file_size; current_byte += 1000) {
				memset(file_desc, 0, COMMON_STRUCT_SIZE);

				fseek(file, current_byte, SEEK_SET);
				fread(file_desc->data, 1, 1000, file);

				if (file_size - current_byte > 1000)
					file_desc->next_descriptor = sector + 2;

				write_to_fs(file_desc, sector * SECTOR_SIZE);

				sector = init->next_free_sector;
				
				if (file_size - current_byte > 1000)
					init->next_free_sector += 2;
			}
		} else {
			while (next_descriptor != 0) {
				memset(file_desc, 0, COMMON_STRUCT_SIZE);
				read_from_fs(file_desc, next_descriptor * SECTOR_SIZE);

				fseek(file, current_byte, SEEK_SET);
				fread(file_desc->data, 1, 1000, file);

				write_to_fs(file_desc, next_descriptor * SECTOR_SIZE);
				printf("Changed sectors at %d\n", next_descriptor);

				current_byte += 1000;

				if (file_desc->next_descriptor == 0)
					break;

				next_descriptor = file_desc->next_descriptor;
			}

			if (size_in_sectors > file_init->size_in_sectors) {
				overwrite_existing = 0;
				file_desc->next_descriptor = sector;
				write_to_fs(file_desc, next_descriptor * SECTOR_SIZE);

				goto backtrack;
			}
		}

		free(file_desc);
	}

	file_init->size_in_sectors = size_in_sectors;
	file_init->bytes_unused_last_kb = bytes_unused_last_kb;

	write_to_fs(file_init, cur_sector * SECTOR_SIZE);
	write_to_fs(init, 0);

	if (file_index > 66)
		write_to_fs(current_dir_list, current_dir_list_sector * SECTOR_SIZE);

	free(file_init);
}