#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fs_file = NULL;
char fs_name[512];

struct initial_directory_listing *init;
struct directory_listing *current_dir_list;

void write_to_fs(void *data, uint16_t where) {
	fseek(fs_file, where, SEEK_SET);
	fwrite(data, 1, COMMON_STRUCT_SIZE, fs_file);
	fclose(fs_file);
	fs_file = fopen(fs_name, "r+");
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

	uint16_t dir_listing_sector = init->next_directory_listing;

	while (dir_listing_sector != 0) {
		fseek(fs_file, dir_listing_sector * SECTOR_SIZE, SEEK_SET);
		fread(current_dir_list, 1, COMMON_STRUCT_SIZE, fs_file);

		for (int i = 0; i < 62; i++)
			if (strcmp(name, current_dir_list->entries[i].name) == 0)
				return i + 66;

		dir_listing_sector = current_dir_list->next_directory_listing;
	}

	return 132;
}

void add_file() {
	printf("Enter name of file to add: ");
	char file_name[512];
	scanf("%s", file_name);
	FILE *file = fopen(file_name, "r");

	if (file == NULL) {
		printf("Unable to open file %s\n", file_name);
		return;
	}

	if (find_file(file_name) == FILE_NOT_FOUND) {
		// Could not find file, allocate space
		if (init->next_free_entry != 66) {
			// Space found for file in initial directory
			uint8_t file_index = init->next_free_entry;

			strcpy(init->entries[init->next_free_entry].name, file_name);
			init->entries[init->next_free_entry].attributes = 0;
			init->entries[init->next_free_entry].sector = init->next_free_sector;
			init->next_free_sector += 2;
			init->next_free_entry++;
			write_to_fs(init, 0);

			fseek(file, 0, SEEK_END);
			size_t file_size = ftell(file);
			fseek(file, 0, SEEK_SET);

			struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);
			file_init->size_in_sectors = (((file_size / 1000) / SECTOR_SIZE) * 2) < 2 ? 2 : (((file_size / 1000) / SECTOR_SIZE) * 2);
			fread(file_init->data, 1, (file_size < 1000 ? file_size : 1000), file);
			write_to_fs(file_init, init->entries[file_index].sector * SECTOR_SIZE);
			
			free(file_init);
			// if (file_size > 1000) {
			// 	for (size_t current_byte = 1000; current_byte < file_size; current_byte += 1000) {
					
			// 	}
			// }
		} else if (current_dir_list->next_free_entry != 66) {
			// Space found for file in current directory listing
		} else {
			// Create new directory listing for file
		}
	}

	// Write contents
}

void file_system_editing() {
	printf("File system editting options:\n1) Add file\n2) Remove file\n3) Add directory of files\n");
	
	char choice;
	do {
		choice = getchar();
		
		switch(choice) {
		case '1':
			add_file();
			goto fs_edit_cycle_complete;

		case '2':
			goto fs_edit_cycle_complete;

		case '3':
			goto fs_edit_cycle_complete;

		}
	} while (choice != '\n' || choice != EOF);

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
	char choice;
	
	do {
		choice = getchar();

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

			goto cycle_complete;
		}

		case '3': {
			printf("Listing of file system:\n");
			
			for (int i = 0; i < 62; i++)
				printf("%s | %d\n", init->entries[i].name, init->entries[i].sector);

			if (init->next_directory_listing == 0)
				goto cycle_complete;

			uint16_t dir_listing_sector = init->next_directory_listing;

			while (dir_listing_sector != 0) {
				fseek(fs_file, dir_listing_sector * SECTOR_SIZE, SEEK_SET);
				fread(current_dir_list, 1, COMMON_STRUCT_SIZE, fs_file);

				for (int i = 0; i < 62; i++)
					printf("%s | %d\n", init->entries[i].name, current_dir_list->entries[i].sector);

				dir_listing_sector = current_dir_list->next_directory_listing;
			}

			goto cycle_complete;
		}

		case 'q':
		case '4':
			free(current_dir_list);
			free(init);
			fclose(fs_file);

			exit(0);

		}
	} while (choice != '\n' || choice != EOF);

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
	fread(init, 1, COMMON_STRUCT_SIZE, fs_file);

	current_dir_list = (struct directory_listing *) malloc(COMMON_STRUCT_SIZE);
	
	no_fs:
	for (;;) options();
}