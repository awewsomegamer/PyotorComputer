#ifndef DISK_H
#define DISK_H

#include <global.h>

void disk_write_buffer(uint16_t length, uint16_t address, uint8_t device, uint16_t device_address);
void disk_read_buffer(uint16_t length, uint16_t address, uint8_t device, uint16_t device_address);

#endif