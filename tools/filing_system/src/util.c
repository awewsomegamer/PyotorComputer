#include "include/main.h"
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

// "Memory Conditional Set"
void memcset(void *buffer, uint8_t value, uint8_t condition, size_t length) {
	for (size_t i = 0; i < length; i++)
		// If the current byte meets the condition, set it
		if (*((uint8_t *)buffer + i) == condition)
			*((uint8_t *)buffer + i) = value;
}

// 0 - 65 : File index in initial directory list
// MAX_FILES_PER_DIR - 131 : File index in current directory list
// 132 : File not found
uint8_t find_file(char *name) {
	// Check initializer directory for the file
	for (int i = 0; i < MAX_FILES_PER_DIR; i++)
		if (strcmp(name, init->entries[i].name) == 0)
			return i;
	
	// If the file is not found, and there is no next directory
	// the file could not be found
	if (init->next_directory_listing == 0)
		return FILE_NOT_FOUND;

	// If there is another directory listing, update the current listing
	// we are looking at
	current_dir_list_sector = init->next_directory_listing;

	// While we can look at a next directory
	while (current_dir_list_sector != 0) {
		// Read the directory data from the file system
		read_from_fs(current_dir_list, current_dir_list_sector * SECTOR_SIZE);

		// Check the files in the directory
		for (int i = 0; i < MAX_FILES_PER_DIR; i++)
			if (strcmp(name, current_dir_list->entries[i].name) == 0)
				return i + MAX_FILES_PER_DIR;
		
		// Go to the next directory
		current_dir_list_sector = current_dir_list->next_directory_listing;
	}

	// File was not found
	return FILE_NOT_FOUND;
}

uint8_t *get_file_data(uint8_t file_index, size_t *size) {
	// Check if the file exists
	if (file_index == FILE_NOT_FOUND) {
		printf("File not found\n");
		return NULL;
	}

	// Buffer in which the data will reside
	uint8_t *buffer = NULL;

	// Create the file initializer
	struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);

	// Check if the file index is in initializer
	if (file_index < MAX_FILES_PER_DIR) {
		// Read the file initializer
		read_from_fs(file_init, init->entries[file_index].sector * SECTOR_SIZE);
	} else {
		// File index is in the current working directory
		read_from_fs(file_init, current_dir_list->entries[file_index - MAX_FILES_PER_DIR].sector * SECTOR_SIZE);
	}

	// Get the size of the file
	*size = (file_init->size_in_sectors / 2) * 1000 - file_init->bytes_unused_last_kb;
	
	// Size is 0? we have no data, return NULL
	if (*size == 0)
		return NULL;

	// Allocate a buffer with a rounded up size
	buffer = malloc((file_init->size_in_sectors / 2) * 1000);
	// Copy initializer's data to the buffer
	memcpy(buffer, file_init->data, 1000);

	// Read in the sector of the next descriptor
	uint16_t next_sector = file_init->next_descriptor;
	
	// Memory manage, the initializer is no longer needed
	free(file_init);

	// If there is no more data to read, return the buffer
	if (next_sector == 0)
		return buffer;
	
	// Allocate a file descriptor
	struct file_descriptor *desc = (struct file_descriptor *) malloc(COMMON_STRUCT_SIZE);
	// The current offset into the buffer
	size_t offset = 1000;
	// While there is a another descriptor to read
	while (next_sector != 0) {
		// Read in the descriptor
		read_from_fs(desc, next_sector * SECTOR_SIZE);
		// Copy over the data to the right offset in the buffer
		memcpy(buffer + offset, desc->data, 1000);

		// Increase the offset
		offset += 1000;
		// Update the next sector to the next descriptor
		next_sector = desc->next_descriptor;
	}
	
	// Memory manage
	free(desc);
	
	// Return the buffer with extended data
	return buffer;

	return NULL;
}

void write_file_data(FILE *file, size_t file_size, uint8_t file_index, uint8_t overwrite_existing) {
	// Create new initializer structure
	struct initial_file_descriptor *file_init = (struct initial_file_descriptor *) malloc(COMMON_STRUCT_SIZE);
	memset(file_init, 0, COMMON_STRUCT_SIZE);
	
	// Calculate relevant information for current file
	uint16_t size_in_sectors = ((file_size + 1000) / 1000) == 1 ? 2 : (((file_size + 1000) / 1000) * 2);
	uint16_t bytes_unused_last_kb = ((size_in_sectors / 2) * 1000) - file_size;
	uint16_t cur_sector = (file_index < MAX_FILES_PER_DIR) ? init->entries[file_index].sector : current_dir_list->entries[file_index - MAX_FILES_PER_DIR].sector;

	// If we are updating file data, we read in previous file data
	if (overwrite_existing)
		read_from_fs(file_init, cur_sector * SECTOR_SIZE);

	// Read first KB of data to the data field
	fread(file_init->data, 1, (file_size < 1000 ? file_size : 1000), file);

	if (file_size > 1000) {
		// The file is larger than 1 KB, initialize a new file descriptor
		struct file_descriptor *file_desc = (struct file_descriptor *) malloc(COMMON_STRUCT_SIZE);

		// Get the next sector we can write to
		uint16_t sector = init->next_free_sector;
		// Get the next pointer for the descriptor
		uint16_t next_descriptor = file_init->next_descriptor;
		// The current starting offset in the file
		size_t current_byte = 1000;

		if (!overwrite_existing) {
			// We are not writing over previous file data
			// point the initializers next link the sector we are writing at
			file_init->next_descriptor = sector;

			backtrack:
			// We are not, or no longer, writing over previous file data

			// Add two sectors to the next free sector of the initializer
			init->next_free_sector += 2;

			// While there is file data to be written
			for (; current_byte < file_size; current_byte += 1000) {
				// Zero the current descriptor
				memset(file_desc, 0, COMMON_STRUCT_SIZE);

				// Seek and read new data
				fseek(file, current_byte, SEEK_SET);
				fread(file_desc->data, 1, 1000, file);

				// If we are not writing the last descriptor, in that
				// there is still more than 1 KB of data to write, point
				// the current link to the next
				if (file_size - current_byte > 1000)
					file_desc->next_descriptor = sector + 2;

				// Write the descriptor to the file system
				write_to_fs(file_desc, sector * SECTOR_SIZE);
				
				// Update the sector to where we are writing
				sector = init->next_free_sector;
				
				// If we are not writing the last descriptor...
				// increment the initial directory listing's
				// next_free_sector
				if (file_size - current_byte > 1000)
					init->next_free_sector += 2;
			}
		} else {
			// We are overwriting file
			while (next_descriptor != 0) {
				// While there is a new link to update, read in the current descriptor
				read_from_fs(file_desc, next_descriptor * SECTOR_SIZE);
				// Set the descriptor's data to 0
				memset(file_desc->data, 0, 1000);

				// Seek the data in the file to update the descriptor with
				fseek(file, current_byte, SEEK_SET);
				// Read in the new data into the descriptor
				if (fread(file_desc->data, 1, 1000, file) == 0) {
					// We read in 0 bytes of data, mark this descriptor as empty
					file_desc->attributes |= 0b01000000;
					
					// Mark the directory_listing_entry as containing empty descriptors as well
					if (file_index > MAX_FILES_PER_DIR)
						current_dir_list->entries[file_index - MAX_FILES_PER_DIR].attributes |= 0b00100000;
					else
						init->entries[file_index].attributes |= 0b00100000;
				}

				// Write the descriptor
				write_to_fs(file_desc, next_descriptor * SECTOR_SIZE);
				// Increment to the next kilobyte sector
				current_byte += 1000;

				// If the next descriptor is 0, break
				if (file_desc->next_descriptor == 0)
					break;
				
				// If the next descriptor isn't 0, update it
				next_descriptor = file_desc->next_descriptor;
			}
			
			// Check if there is more data to write
			if (size_in_sectors > file_init->size_in_sectors) {
				// If so, we are adding descriptors, clear overwrite_existing
				overwrite_existing = 0;
				// Point the current descriptors link to the next free sector
				file_desc->next_descriptor = sector;
				// Write the descriptor to the file system
				write_to_fs(file_desc, next_descriptor * SECTOR_SIZE);

				// Backtrack
				goto backtrack;
			}
		}

		// Memory manage
		free(file_desc);
	}

	// Update the file initializer's metadata
	file_init->size_in_sectors = size_in_sectors;
	file_init->bytes_unused_last_kb = bytes_unused_last_kb;

	// Write file initializer and initial directory listing to file system
	write_to_fs(file_init, cur_sector * SECTOR_SIZE);
	write_to_fs(init, 0);
	
	// If the file is not contained in the initializer then update its directory
	// listing descriptor
	if (file_index > MAX_FILES_PER_DIR)
		write_to_fs(current_dir_list, current_dir_list_sector * SECTOR_SIZE);

	// Memory manageb
	free(file_init);
}