#include "main.h"
#include <stdlib.h>

void delete_file(char *file_name) {
	uint8_t alloc_name = 0;
	if (file_name == NULL) {
		printf("Enter name of file to delete: ");
		file_name = malloc(512);
		alloc_name = 1;
		scanf("%s", file_name);
	}

	uint8_t file_index = 132;

	if ((file_index = find_file(file_name)) == FILE_NOT_FOUND) {
		printf("File not found\n");
		
		if (alloc_name)
			free(file_name);

		return;
	}

	uint16_t sector = 0;
	if (file_index < MAX_FILES_PER_DIR) {
		// In initial directory
		init->entries[file_index].attributes |= 0b10000000; // | D
		sector = init->entries[file_index].sector;
		write_to_fs(init, 0);
	} else {
		// In external directory
		file_index -= MAX_FILES_PER_DIR;
		current_dir_list->entries[file_index].attributes |= 0b10000000; // | D
		sector = current_dir_list->entries[file_index].sector;
		write_to_fs(current_dir_list, 0);
	}

	struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);
	read_from_fs(file_init, sector * SECTOR_SIZE);
	file_init->attributes |= 0b10000000; // | D
	write_to_fs(file_init, sector * SECTOR_SIZE);

	uint16_t next_descriptor = file_init->next_descriptor;
	
	free(file_init);

	if (file_init->next_descriptor == 0) {
		printf("Successfully marked file %s as deleted\n", file_name);

		if (alloc_name)
			free(file_name);

		return;
	}

	struct file_descriptor *file_desc = (struct file_descriptor *) malloc(COMMON_STRUCT_SIZE);

	while (next_descriptor != 0) {
		read_from_fs(file_desc, next_descriptor * SECTOR_SIZE);
		file_desc->attributes |= 0b10000000; // | D
		write_to_fs(file_desc, next_descriptor * SECTOR_SIZE);

		next_descriptor = file_desc->next_descriptor;
	}

	free(file_desc);

	if (alloc_name)
		free(file_name);

	printf("Successfully marked file %s as deleted\n", file_name);
}