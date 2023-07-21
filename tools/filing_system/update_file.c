#include "main.h"

void update_file(char *file_name) {
	uint8_t alloc_name = 0;

	if (file_name == NULL) {
		printf("Enter name of file to update: ");
		file_name = malloc(512);
		alloc_name = 1;
		scanf("%s", file_name);
	}

	FILE *file = fopen(file_name, "r");

	int name_offset = strlen(file_name) - 1;
	for (; (file_name[name_offset] != '/') && name_offset >= 0; name_offset--);
	char *name = malloc(13);
	strncpy(name, file_name, 13);
	
	if (name_offset <= 0) {
		memset(name, 0, 13);
		strncpy(name, file_name + name_offset + 1, 13);
	}

	if (file == NULL) {
		printf("Unable to open file %s\n", file_name);
		free(name);
		
		if (alloc_name)
			free(file_name);

		return;
	}

	uint8_t file_index = find_file(name);

	if (file_index < MAX_FILES_PRE_DIR) {
		// File is in the intial directory
		fseek(file, 0, SEEK_END);
		size_t file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		write_file_data(file, file_size, file_index, 1);

		free(name);

		if (alloc_name)
			free(file_name);

		fclose(file);
	} else if (file_index < FILE_NOT_FOUND) {
		// File is in the current direcotry
	} else {
		printf("File not found\n");
	}
}