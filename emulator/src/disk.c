#include "include/disk.h"
#include "include/ram.h"
#include <math.h>

FILE *connected_disks[3];

uint8_t disk_operation_buffer(uint16_t sectors, uint16_t address, uint8_t device, uint16_t sector, uint8_t operation) {
	int disk_index = log2(device & 0b111);

	if (disk_index > 2 || disk_index < 0) {
		DBG(1, printf("Disk index %d is not a valid disk index", disk_index);)
		return -1; // Multiple or no disks are bound for operation
	}

	if (connected_disks[disk_index] == NULL) {
		DBG(1, printf("Disk %d is not connected", disk_index);)
		return -1; // The selected disk is not connected to the system
	}

	fseek(connected_disks[disk_index], sector * SECTOR_SIZE, SEEK_SET);
	
	if (operation == 1) {
		DBG(1, printf("Writing disk %d at sector 0x%04X from memory 0x%04X for %d sectors", disk_index, sector, address, sectors);)
		fwrite(general_memory + address, 1, sectors * SECTOR_SIZE, connected_disks[disk_index]);
	} else {
		DBG(1, printf("Reading disk %d at sector 0x%04X to memory 0x%04X for %d sectors", disk_index, sector, address, sectors);)
		fread(general_memory + address, 1, sectors * SECTOR_SIZE, connected_disks[disk_index]);
	}

	DBG(1, printf("Disk operation completed successfully");)

	return disk_index;
}

void connect_disk(char *name, uint8_t number) {
	DBG(1, printf("Connecting file \"%s\" to disk number %d", name, number);)
	connected_disks[number] = fopen(name, "r+");
	ASSERT(connected_disks[number] != NULL)
	DBG(1, printf("Disk connected");)
}

void disconnect_disk(uint8_t number) {
	fclose(connected_disks[number]);
	connected_disks[number] = NULL;
	DBG(1, printf("Disconnected disk %d", number);)
}

void disconnect_all() {
	for (int i = 0; i < 3; i++) {
		if (connected_disks[i] == NULL)
			continue;

		fclose(connected_disks[i]);
		connected_disks[i] = NULL;
		DBG(1, printf("Disconnected disk %d", i);)
	}
}