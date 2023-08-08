#ifndef DISK_H
#define DISK_H

#include "global.h"

#define SECTOR_SIZE 512

uint8_t disk_operation_buffer(uint16_t sectors, uint16_t address, uint8_t device, uint16_t sector, uint8_t operation);
void connect_disk(char *name, uint8_t number);
void disconnect_disk(uint8_t number);
void disconnect_all();

#endif