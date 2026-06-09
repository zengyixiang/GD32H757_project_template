#include "at24c02.h"

static uint16_t at24c02_min_u16(uint16_t left, uint16_t right)
{
    return (left < right) ? left : right;
}

int at24c02_init(at24c02_t *eeprom, const at24c02_config_t *config)
{
    if((eeprom == 0) || (config == 0) || (config->mem_write == 0) || (config->mem_read == 0)) {
        return -1;
    }

    eeprom->config = *config;

    if(eeprom->config.device_address == 0U) {
        eeprom->config.device_address = AT24C02_DEFAULT_ADDRESS;
    }
    if(eeprom->config.capacity_bytes == 0U) {
        eeprom->config.capacity_bytes = AT24C02_CAPACITY_BYTES;
    }
    if(eeprom->config.page_size == 0U) {
        eeprom->config.page_size = AT24C02_PAGE_SIZE;
    }

    return 0;
}

int at24c02_read(at24c02_t *eeprom, uint16_t memory_address, uint8_t *data, uint16_t size)
{
    if((eeprom == 0) || ((data == 0) && (size > 0U))) {
        return -1;
    }
    if(((uint32_t)memory_address + size) > eeprom->config.capacity_bytes) {
        return -2;
    }

    return eeprom->config.mem_read(eeprom->config.bus_context, eeprom->config.device_address,
                                   memory_address, data, size);
}

int at24c02_write(at24c02_t *eeprom, uint16_t memory_address, const uint8_t *data, uint16_t size)
{
    uint16_t current_address = memory_address;
    uint16_t remaining = size;
    const uint8_t *current_data = data;

    if((eeprom == 0) || ((data == 0) && (size > 0U))) {
        return -1;
    }
    if(((uint32_t)memory_address + size) > eeprom->config.capacity_bytes) {
        return -2;
    }

    while(remaining > 0U) {
        uint16_t page_offset = current_address % eeprom->config.page_size;
        uint16_t chunk_size = at24c02_min_u16((uint16_t)(eeprom->config.page_size - page_offset),
                                             remaining);
        int written = eeprom->config.mem_write(eeprom->config.bus_context,
                                               eeprom->config.device_address,
                                               current_address, current_data, chunk_size);

        if(written != (int)chunk_size) {
            return -3;
        }

        if(eeprom->config.delay_ms != 0) {
            eeprom->config.delay_ms(eeprom->config.write_cycle_ms);
        }

        current_address = (uint16_t)(current_address + chunk_size);
        current_data += chunk_size;
        remaining = (uint16_t)(remaining - chunk_size);
    }

    return (int)size;
}
