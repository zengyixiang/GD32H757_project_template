#ifndef STORAGE_SERVICE_H
#define STORAGE_SERVICE_H

#include <stdint.h>

void storage_service_init(void);
int storage_service_read_byte(uint16_t address, uint8_t *value);
int storage_service_write_byte(uint16_t address, uint8_t value);

#endif
