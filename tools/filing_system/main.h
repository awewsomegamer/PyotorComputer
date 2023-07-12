#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define SECTOR_SIZE 512
#define FILE_NOT_FOUND 132
#define COMMON_STRUCT_SIZE 1024

struct directory_listing_entry {
	char name[13]; // The name by which the file can be found
	uint16_t sector; // The sector at which the initial file descriptor is located
	uint8_t attributes; // D E 0 0 0 0 0 0
			    // (D)eleted - 1 deleted, 0 not deleted
			    // (E)xists - 1 entry exists, 0 entry does not exist
}__attribute__((packed, aligned(16))); // 16 byte fields

struct initial_directory_listing {
	struct directory_listing_entry entries[62]; // Uses the first ~1000 bytes of data
	uint16_t next_free_sector; // The next free sector on disk
	uint8_t next_free_entry; // Next free entry in this directory
	uint16_t next_directory_listing; // Pointer to the next sector containing directory listing
}__attribute__((packed, aligned(1024))); // 1024 byte field

struct directory_listing {
	struct directory_listing_entry entries[62]; // Uses the first ~1000 bytes of data
	uint8_t next_free_entry; // Next free entry in this directory
	uint16_t next_directory_listing; // Pointer to next sector containing next directory listing
}__attribute__((packed, aligned(1024))); // 1024 byte field

struct initial_file_descriptor {
	uint8_t data[1000]; // 1000 bytes of file data
	uint16_t next_descriptor; // Pointer to the next file descriptor
	uint16_t size_in_sectors; // The total size of the file in sectors
	uint8_t attributes; // D 0 0 0 0 0 0 0
			    // D(eleted) - 1 deleted, 0 not deleted
}__attribute__((packed, aligned(1024))); // 1024 byte field

struct file_descriptor {
	uint8_t data[1000]; // 1000 bytes of file data
	uint16_t next_descriptor; // The next file descriptor
	uint8_t attributes; // D 0 0 0 0 0 0 0
			    // D(eleted) - 1 deleted, 0 not deleted
}__attribute__((packed, aligned(1024)));

#endif