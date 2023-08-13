#include "include/main.h"
#include <stdio.h>
#include <string.h>

void add_file(char *file_name) {
	uint8_t alloc_name = 0;

	if (file_name == NULL) {
		printf("Enter name of file to add: ");
		file_name = malloc(512);
		alloc_name = 1;
		scanf("%s", file_name);
	}

	FILE *file = fopen(file_name, "r");

	int name_offset = strlen(file_name) - 1;
	for (; (file_name[name_offset] != '/') && name_offset >= 0; name_offset--);
	char *name = malloc(13);
	strncpy(name, file_name, 13);
	
	if (name_offset > 0) {
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

	if (find_file(file_name) == FILE_NOT_FOUND) {
		// Could not find file, allocate space
		uint8_t file_index = 0;

		if (init->next_free_entry != MAX_FILES_PER_DIR) {
			// Space found for file in initial directory
			file_index = init->next_free_entry;

			strcpy(init->entries[init->next_free_entry].name, name);
			init->entries[init->next_free_entry].attributes = 0b01000000; // | E
			init->entries[init->next_free_entry].sector = init->next_free_sector;
			init->next_free_sector += 2;
			init->next_free_entry++;
		} else if (current_dir_list->next_free_entry != MAX_FILES_PER_DIR) {
			directory_listing_case:
			// Space found for file in current directory listing
			file_index = current_dir_list->next_free_entry;

			strcpy(current_dir_list->entries[init->next_free_entry].name, name);
			current_dir_list->entries[init->next_free_entry].attributes = 0b01000000; // | E
			current_dir_list->entries[init->next_free_entry].sector = init->next_free_sector;
			init->next_free_sector += 2;
			current_dir_list->next_free_entry++;

			write_to_fs(current_dir_list, current_dir_list_sector * SECTOR_SIZE);
		} else {
			// Weird bug here, initial directory listing is absolutely destroyed
			// Create new directory listing for file
			current_dir_list_sector = init->next_free_sector;

			if (init->next_directory_listing == 0) {
				// Next directory listing follows initial listing
				init->next_directory_listing = current_dir_list_sector;
			} else {
				// Next directory listing doesn't follow initial listing
				current_dir_list->next_directory_listing = current_dir_list_sector;
			}

			init->next_free_sector += 2;
			memset(current_dir_list, 0, COMMON_STRUCT_SIZE);
			goto directory_listing_case;
		}

		fseek(file, 0, SEEK_END);
		size_t file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		write_file_data(file, file_size, file_index, 0);
		
		free(name);

		fclose(file);

		printf("File %s was written to file system, initial descriptor is located in sector %d\n", file_name, init->entries[file_index].sector);

		if (alloc_name)
			free(file_name);

		return;
	}

	free(name);

	if (alloc_name)
		free(file_name);

	fclose(file);
	printf("File already exists, use option \"Update file\"\n");
}

void add_dir(char *path) {
	DIR *d = opendir(path);
	struct dirent *dir_ent;

	while ((dir_ent = readdir(d)) != NULL) {
		if (dir_ent->d_type == DT_REG) {
			if (find_file(dir_ent->d_name) != FILE_NOT_FOUND) {
				// Update file
			}

			char *n_path = malloc(strlen(path) + strlen(dir_ent->d_name) + 1);
			strcpy(n_path, path);
			n_path[strlen(path)] = '/';
			strcpy(n_path + strlen(path) + 1, dir_ent->d_name);

			add_file(n_path);

			free(n_path);
		}
	}

	closedir(d);
}

void recursive_add_dir(char *path) {
	DIR *d = opendir(path);
	struct dirent *dir_ent;

	add_dir(path);

	while ((dir_ent = readdir(d)) != NULL) {
		if (dir_ent->d_type == DT_DIR && strcmp(dir_ent->d_name, "..") != 0 && strcmp(dir_ent->d_name, ".") != 0) {
			char *n_path = malloc(strlen(path) + strlen(dir_ent->d_name) + 1);
			strcpy(n_path, path);
			n_path[strlen(path)] = '/';
			strcpy(n_path + strlen(path) + 1, dir_ent->d_name);

			recursive_add_dir(n_path);

			free(n_path);
		}
	}

	closedir(d);
}

void add_files_in_dir() {
	printf("Recursively add files in sub-folders (y/n)?: ");

	int recurse = -1;
	for (; recurse == -1;)
		recurse = ((recurse = get_char()) == 'y' || (recurse == 'Y')) ? 1 : 0;

	printf("Enter directory path of files to add: ");
	char path[512];
	scanf("%s", path);

	// Scan lower, sub, folders, add all files in a linear fashion
	if (recurse) {
		recursive_add_dir(path);
		return;
	}

	// Just add files in given directory
	add_dir(path);
}