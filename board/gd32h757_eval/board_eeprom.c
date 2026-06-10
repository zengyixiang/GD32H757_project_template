#include "board_eeprom.h"

#include "at24c02.h"
#include "board_i2c.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>

static at24c02_t board_eeprom;

static int board_at24c02_mem_write(void *context, uint8_t device_address,
                                   uint16_t memory_address, const uint8_t *data,
                                   uint16_t size)
{
    board_i2c_id_t i2c_id = (board_i2c_id_t)(uintptr_t)context;

    return board_i2c_mem_write(i2c_id, device_address, memory_address, data, size);
}

static int board_at24c02_mem_read(void *context, uint8_t device_address,
                                  uint16_t memory_address, uint8_t *data,
                                  uint16_t size)
{
    board_i2c_id_t i2c_id = (board_i2c_id_t)(uintptr_t)context;

    return board_i2c_mem_read(i2c_id, device_address, memory_address, data, size);
}

static void board_at24c02_delay_ms(uint32_t timeout_ms)
{
    vTaskDelay(pdMS_TO_TICKS(timeout_ms));
}

void board_eeprom_init(void)
{
    const at24c02_config_t config = {
        .device_address = AT24C02_DEFAULT_ADDRESS,
        .capacity_bytes = AT24C02_CAPACITY_BYTES,
        .page_size = AT24C02_PAGE_SIZE,
        .write_cycle_ms = 5U,
        .bus_context = (void *)(uintptr_t)BOARD_I2C_EEPROM,
        .mem_write = board_at24c02_mem_write,
        .mem_read = board_at24c02_mem_read,
        .delay_ms = board_at24c02_delay_ms,
    };

    (void)at24c02_init(&board_eeprom, &config);
}

int board_eeprom_read(uint16_t memory_address, uint8_t *data, uint16_t size)
{
    return at24c02_read(&board_eeprom, memory_address, data, size);
}

int board_eeprom_write(uint16_t memory_address, const uint8_t *data, uint16_t size)
{
    return at24c02_write(&board_eeprom, memory_address, data, size);
}
