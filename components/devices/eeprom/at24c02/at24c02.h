#ifndef AT24C02_H
#define AT24C02_H

#include <stdint.h>

#define AT24C02_DEFAULT_ADDRESS 0x50U
#define AT24C02_CAPACITY_BYTES 256U
#define AT24C02_PAGE_SIZE 8U

typedef int (*at24c02_mem_write_fn)(void *context, uint8_t device_address,
                                    uint16_t memory_address, const uint8_t *data,
                                    uint16_t size);
typedef int (*at24c02_mem_read_fn)(void *context, uint8_t device_address,
                                   uint16_t memory_address, uint8_t *data,
                                   uint16_t size);
typedef void (*at24c02_delay_ms_fn)(uint32_t timeout_ms);

typedef struct {
    uint8_t device_address;
    uint16_t capacity_bytes;
    uint8_t page_size;
    uint8_t write_cycle_ms;
    void *bus_context;
    at24c02_mem_write_fn mem_write;
    at24c02_mem_read_fn mem_read;
    at24c02_delay_ms_fn delay_ms;
} at24c02_config_t;

typedef struct {
    at24c02_config_t config;
} at24c02_t;

int at24c02_init(at24c02_t *eeprom, const at24c02_config_t *config);
int at24c02_read(at24c02_t *eeprom, uint16_t memory_address, uint8_t *data, uint16_t size);
int at24c02_write(at24c02_t *eeprom, uint16_t memory_address, const uint8_t *data, uint16_t size);

#endif
