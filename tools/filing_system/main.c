#include "main.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *fs_file = NULL;
char fs_name[512];

struct initial_directory_listing *init;
struct directory_listing *current_dir_list;
uint16_t current_dir_list_sector;

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

void add_file(char *file_name) {
	if (file_name == NULL) {
		printf("Enter name of file to add: ");
		file_name = malloc(512);
		scanf("%s", file_name);
	}

	FILE *file = fopen(file_name, "r");

	printf("Error B1 %s\n", file_name);
	char *name = malloc(13);
	memset(name, 0, 13);
	int name_offset = strlen(file_name) - 1;
	for (; (file_name[name_offset] != '/'); name_offset--);
	strncpy(name, file_name + name_offset + 1, 13);
	printf("Error B2\n");

	if (file == NULL) {
		printf("Unable to open file %s\n", file_name);
		free(name);
		fclose(file);
		return;
	}

	if (find_file(file_name) == FILE_NOT_FOUND) {
		// Could not find file, allocate space
		uint8_t file_index = 0;

		if (init->next_free_entry != 66) {
			// Space found for file in initial directory
			file_index = init->next_free_entry;

			strcpy(init->entries[init->next_free_entry].name, name);
			init->entries[init->next_free_entry].attributes = 0b01000000; // | E
			init->entries[init->next_free_entry].sector = init->next_free_sector;
			init->next_free_sector += 2;
			init->next_free_entry++;
		} else if (current_dir_list->next_free_entry != 66) {
			// Space found for file in current directory listing
			file_index = init->next_free_entry;

			strcpy(current_dir_list->entries[init->next_free_entry].name, name);
			current_dir_list->entries[init->next_free_entry].attributes = 0b01000000; // | E
			current_dir_list->entries[init->next_free_entry].sector = init->next_free_sector;
			init->next_free_sector += 2;
			init->next_free_entry++;

			write_to_fs(current_dir_list, current_dir_list_sector * SECTOR_SIZE);
		} else {
			// Create new directory listing for file
			if (init->next_directory_listing == 0) {
				// Next directory listing follows initial listing
			} else {
				// Next directory listing doesn't follow initial listing
			}
		}

		fseek(file, 0, SEEK_END);
		size_t file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);
		memset(file_init, 0, COMMON_STRUCT_SIZE);
		file_init->size_in_sectors = (((file_size / 1000) / SECTOR_SIZE) * 2) < 2 ? 2 : (((file_size / 1000) / SECTOR_SIZE) * 2);
		fread(file_init->data, 1, (file_size < 1000 ? file_size : 1000), file);

		if (file_size > 1000) {
			uint16_t sector = init->next_free_sector;
			file_init->next_descriptor = sector;
			init->next_free_sector += 2;

			struct file_descriptor *file_desc = (struct file_descriptor *) malloc(COMMON_STRUCT_SIZE);

			for (size_t current_byte = 1000; current_byte < file_size; current_byte += 1000) {
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

			free(file_desc);
		}

		write_to_fs(file_init, init->entries[file_index].sector * SECTOR_SIZE);
		write_to_fs(init, 0);
		
		free(name);
		free(file_init);
		fclose(file);

		printf("File %s was written to file system, initial descriptor is located in sector %d\n", file_name, init->entries[file_index].sector);

		return;
	}

	free(name);
	fclose(file);
	printf("File already exists, use option \"Update file\"\n");
}

void update_file() {

}

void delete_file() {
	printf("Enter name of file to delete: ");
	char file_name[512];
	scanf("%s", file_name);
	
	uint8_t file_index = 132;

	if ((file_index = find_file(file_name)) == FILE_NOT_FOUND) {
		printf("File not found\n");
		return;
	}

	uint16_t sector = 0;
	if (file_index < 66) {
		// In initial directory
		init->entries[file_index].attributes |= 0b10000000; // | D
		sector = init->entries[file_index].sector;
		write_to_fs(init, 0);
	} else {
		// In external directory
		file_index -= 66;
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

	printf("Successfully marked file %s as deleted\n", file_name);
}

void add_dir(char *path) {
	DIR *d = opendir(path);
	struct dirent *dir_ent;

	while ((dir_ent = readdir(d)) != NULL) {
		if (dir_ent->d_type == DT_REG) {
			if (find_file(dir_ent->d_name) != FILE_NOT_FOUND) {
				// Update file
			}

			printf("Error A1\n");
			char *n_path = malloc(strlen(path) + strlen(dir_ent->d_name) + 1);
			strcpy(n_path, path);
			n_path[strlen(path)] = '/';
			strcpy(n_path + strlen(path) + 1, dir_ent->d_name);
			printf("Error A2\n");

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
			printf("Error C1 %d %d\n", dir_ent->d_type, DT_DIR);
			char *n_path = malloc(strlen(path) + strlen(dir_ent->d_name) + 1);
			strcpy(n_path, path);
			n_path[strlen(path)] = '/';
			strcpy(n_path + strlen(path) + 1, dir_ent->d_name);
			printf("Error C2 %s\n", n_path);

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

void file_system_editing() {
	printf("File system editting options:\n1) Add file\n2) Update file\n3) Delete file\n4) Add directory of files\n");
	
	for (;;) {
		char choice = get_char();
		
		switch(choice) {
		case '1':
			add_file(NULL);
			goto fs_edit_cycle_complete;
		
		case '2':
			goto fs_edit_cycle_complete;

		case '3':
			delete_file();
			goto fs_edit_cycle_complete;

		case '4':
			add_files_in_dir();
			goto fs_edit_cycle_complete;

		}
	}

	fs_edit_cycle_complete:;
}

void options() {
	// Options:
	// Modify existing file system
	//	Add file
	//	Remove file
	//	Add directory of files
	// Create brand new, never before seen file system
	// Directory listing
	printf("Options menu:\n1) Modify exsisting file system\n2) Create new file system\n3) Directory listing\n4 / q) Quit\n");
	
	for (;;) {
		char choice = get_char();

		switch(choice) {
		case '1':
			file_system_editing();	

			goto cycle_complete;

		case '2': {
			// printf("Destroying file system located in file %s in 15 seconds\n", fs_name);
			// sleep(15);

			if (init == NULL)
				init = (struct initial_directory_listing *) malloc(COMMON_STRUCT_SIZE);

			memset(init, 0, 1024);

			init->next_free_sector = 0x02;
			init->next_free_entry = 0x00;
			init->next_directory_listing = 0x0000;
		
			write_to_fs(init, 0);

			printf("New file system created\n");

			goto cycle_complete;
		}

		case '3': {
			printf("Listing of file system:\n--------------+-------------------\nName          | Init Sector Number\n--------------+-------------------\n");
			
			for (int i = 0; i < 62; i++) {
				if (((init->entries[i].attributes >> 6) & 1) == 1) { // Exists?
					char *buffer = malloc(14);
					memset(buffer, 0, 14);
					strncpy(buffer, init->entries[i].name, 13);
					memcset(buffer, ' ', 0, 13);
					
					printf("%s | %d\n", buffer, init->entries[i].sector);
					free(buffer);
				}
			}

			if (init->next_directory_listing == 0) {
				printf("--------------+-------------------\n");
				goto cycle_complete;
			}

			uint16_t dir_listing_sector = init->next_directory_listing;

			while (dir_listing_sector != 0) {
				fseek(fs_file, dir_listing_sector * SECTOR_SIZE, SEEK_SET);
				fread(current_dir_list, 1, COMMON_STRUCT_SIZE, fs_file);

				for (int i = 0; i < 62; i++) {
					if (((current_dir_list->entries[i].attributes >> 6) & 1) == 1) { // Exists?
						char *buffer = malloc(14);
						memset(buffer, 0, 14);
						strncpy(buffer, init->entries[i].name, 13);
						memcset(buffer, ' ', 0, 13);
						
						printf("%s | %d\n", buffer, init->entries[i].sector);
						free(buffer);
					}
				}

				dir_listing_sector = current_dir_list->next_directory_listing;
			}
			
			printf("--------------+-------------------\n");

			goto cycle_complete;
		}

		case 'q':
		case '4':
			free(current_dir_list);
			free(init);
			fclose(fs_file);

			exit(0);
		}
	}

	cycle_complete:;
}

int main() {
	printf("Now I am become file system, the organizer of disks\n"); // Oppenheimer reference
	printf("Enter the filename used to store the file system: ");
	scanf("%s", fs_name); // Chance buffer overflow
	fs_file = fopen(fs_name, "r+");

	if (fs_file == NULL) {
		fs_file = fopen(fs_name, "w+");
		printf("File %s not found, creating it\n", fs_name);
		goto no_fs;
	}

	init = (struct initial_directory_listing *) malloc(COMMON_STRUCT_SIZE);
	read_from_fs(init, 0);

	current_dir_list = (struct directory_listing *) malloc(COMMON_STRUCT_SIZE);
	
	no_fs:
	for (;;) options();
}