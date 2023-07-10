#include "main.h"
#include <stdio.h>
#include <stdlib.h>

void options() {
	// Options:
	// Modify existing file system
	//	Add file
	//	Remove file
	//	Add directory of files
	// Create brand new, never before seen file system
	printf("Options menu:\n1) Modify exsisting file system\n2) Create new file system\n");
	char choice = getchar();

	switch(choice) {
	case '1':
		printf("Whoops, have not implemented that yet.\n");
		break;
	case '2':
		printf("Enter the filename used to store the file system: ");
		char name[512];
		scanf("%s", name); // Chance buffer overflow
		FILE *fs_file = fopen(name, "w");

		struct initial_directory_listing *init = (struct initial_directory_listing *)malloc(sizeof(struct initial_directory_listing));
		init->next_free_sector = 0x03;

		fwrite(init, 1, sizeof(struct initial_directory_listing), fs_file);
		fclose(fs_file);
		free(init);
		
		break;
	}
}

int main() {
	printf("Now I am become file system, the organizer of disks\n"); // Oppenheimer reference
	
	for (;;) options();
}