#include "storage_service.h"

#include "board_eeprom.h"

void storage_service_init(void)
{
}

int storage_service_read_byte(uint16_t address, uint8_t *value)
{
    return board_eeprom_read(address, value, 1U);
}

int storage_service_write_byte(uint16_t address, uint8_t value)
{
    return board_eeprom_write(address, &value, 1U);
}
