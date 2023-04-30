#ifndef DISK_H
#define DISK_H

#include <global.h>

uint8_t disk_operation_buffer(uint16_t length, uint16_t address, uint8_t device, uint16_t device_address, uint8_t operation);
void connect_disk(char *name, uint8_t number);
void disconnect_disk(uint8_t number);

#endif