#ifndef BOARD_EEPROM_H
#define BOARD_EEPROM_H

#include <stdint.h>

void board_eeprom_init(void);
int board_eeprom_read(uint16_t memory_address, uint8_t *data, uint16_t size);
int board_eeprom_write(uint16_t memory_address, const uint8_t *data, uint16_t size);

#endif
