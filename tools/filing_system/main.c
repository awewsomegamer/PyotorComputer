#include "main.h"
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

void update_file(char *file_name) {
	uint8_t alloc_name = 0;

	if (file_name == NULL) {
		printf("Enter name of file to update: ");
		file_name = malloc(512);
		alloc_name = 1;
		scanf("%s", file_name);
	}

	// Do stuff
}

uint8_t *get_file_data(uint8_t file_index, size_t *size) {
	if (file_index == FILE_NOT_FOUND) {
		printf("File not found\n");
		return NULL;
	}

	uint8_t *buffer;

	if (file_index < 66) {
		struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);
		read_from_fs(file_init, init->entries[file_index].sector * SECTOR_SIZE);

		printf("D: %d\n", (file_init->size_in_sectors / 2) * 1000);

		// *size = (file_init->size_in_sectors / 2) * 1000;
		if ((file_init->size_in_sectors / 2) * 1000 == 0)
			return NULL;

		buffer = malloc((file_init->size_in_sectors / 2) * 1000);
		memccpy(buffer, file_init->data, 1, 1000);

		uint16_t next_sector = file_init->next_descriptor;

		free(file_init);

		if (next_sector == 0)
			return buffer;
		
		struct file_descriptor *desc = (struct file_descriptor *) malloc(COMMON_STRUCT_SIZE);
		size_t offset = 1000;
		while (next_sector != 0) {
			read_from_fs(desc, next_sector * SECTOR_SIZE);
			memccpy(buffer + offset, desc->data, 1, 1000);
			
			offset += 1000;
			next_sector = desc->next_descriptor;
		}

		free(desc);

		return buffer;
	}

	return NULL;
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
			delete_file(NULL);
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
			char file_path[525]; // Cannot do malloc, for some reason I get error at comment "A"
					     // saying malloc(): corrupted top size
					     // Aborted (core dumped)
			scanf("%s", path);
			
			FILE *file;
			for (int i = 0; i < 66; i++) {
				if (((init->entries[i].attributes >> 6) & 1) == 1) {
					size_t size = 0;
					uint8_t *buffer = get_file_data(i, &size);

					if (buffer == NULL)
						continue;
					
					// A
					strcpy(file_path, path);
					file_path[strlen(path)] = '/';
					strncpy(file_path + strlen(path) + 1, init->entries[i].name, 13);
					// file = fopen(_file_path, "w"); // Breaks it
					memset(file_path, 0, 525);

					// fwrite(buffer, 1, size, file);
					// fclose(file);
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