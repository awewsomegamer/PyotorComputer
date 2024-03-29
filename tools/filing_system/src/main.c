#include "include/main.h"
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *fs_file = NULL;
char fs_name[512];

struct initial_directory_listing *init;
struct directory_listing *current_dir_list;
uint16_t current_dir_list_sector;

const uint8_t version_high = 1;
const uint8_t version_low = 0;
const char *identifier = "SFS ";

void file_system_editing() {
	printf("File system editting options:\n1) Add file\n2) Update file\n3) Delete file\n4) Rename a file\n5) Add directory of files\n");
	
	for (;;) {
		char choice = get_char();
		
		switch(choice) {
		case '1':
			add_file(NULL);
			goto fs_edit_cycle_complete;
		
		case '2':
			update_file(NULL);
			goto fs_edit_cycle_complete;

		case '3':
			delete_file(NULL);
			goto fs_edit_cycle_complete;

		case '4': {
			printf("Enter name of file to rename: ");
			char *file_name = malloc(512);
			scanf("%s", file_name);
			
			printf("Enter the new name of file: ");
			char *new_name = malloc(512);
			scanf("%s", new_name);

			uint8_t index = find_file(file_name);

			if (index == FILE_NOT_FOUND) {
				printf("Unable to find file %s\n", file_name);
				free(file_name);
				free(new_name);
			}

			if (index < MAX_FILES_PER_DIR) {
				strncpy(init->entries[index].name, new_name, 13);
				write_to_fs(init, 0);
			} else {
				strncpy(current_dir_list->entries[index - MAX_FILES_PER_DIR].name, new_name, 13);
				write_to_fs(current_dir_list, current_dir_list_sector * SECTOR_SIZE);
			}

			printf("Renamed file %s to %s\n", file_name, new_name);
			free(file_name);
			free(new_name);

			goto fs_edit_cycle_complete;
		}

		case '5':
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
	printf("Options menu:\n1) Modify exsisting file system\n2) Create new file system\n3) Directory listing\n4) Clone file system\n5 / q) Quit\n");
	
	for (;;) {
		char choice = get_char();

		switch(choice) {
		case '1':
			file_system_editing();	

			goto cycle_complete;

		case '2': {
			// printf("Creating new file system located in file %s in 15 seconds\n", fs_name);
			// sleep(15);

			if (init == NULL)
				init = (struct initial_directory_listing *) malloc(COMMON_STRUCT_SIZE);

			memset(init, 0, 1024);

			init->next_free_sector = 0x02;
			init->next_free_entry = 0x00;
			init->next_directory_listing = 0x0000;
			init->version_high = version_high;
			init->version_low = version_low;
			strcpy(init->identifier, identifier);
		
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

		case '4': {
			printf("Enter the directory to clone the file system to: ");
			char path[512];
			scanf("%s", path);
			
			FILE *file;
			for (int i = 0; i < MAX_FILES_PER_DIR; i++) {
				if (((init->entries[i].attributes >> 6) & 1) == 1) {
					size_t size = 0;
					uint8_t *buffer = get_file_data(i, &size);
					
					if (buffer == NULL) {
						printf("%d NULL\n", i);
						continue;
					}

					char *file_path = malloc(512 + strlen(init->entries[i].name));
					memset(file_path, 0, 512 + strlen(init->entries[i].name));
					strcpy(file_path, path);

					file_path[strlen(path)] = '/';
					
					strncpy(file_path + strlen(path) + 1, init->entries[i].name, 13);
					
					file = fopen(file_path, "w");

					if (file == NULL) {
						printf("Cannot open file at %s, please create this directory\n", file_path);
						goto cycle_complete;
					}
					
					fwrite(buffer, 1, size, file);
					fclose(file);
					
					free(file_path);
					free(buffer);
				}
			}

			goto cycle_complete;
		}

		case 'q':
		case '5':
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
	printf("%s Version %d.%d\n", identifier, version_high, version_low);
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