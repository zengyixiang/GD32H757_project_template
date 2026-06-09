#include "bsp_i2c.h"

void bsp_i2c_init(bsp_i2c_t *i2c, const bsp_i2c_config_t *config)
{
    if((i2c == 0) || (config == 0)) {
        return;
    }

    i2c->config = *config;
    rcu_periph_clock_enable(i2c->config.clock);

    (void)i2c->config.i2c_periph;
    (void)i2c->config.speed_hz;
}

int bsp_i2c_write(const bsp_i2c_t *i2c, uint8_t address, const uint8_t *data, uint16_t size)
{
    if((i2c == 0) || ((data == 0) && (size > 0U))) {
        return -1;
    }

    (void)i2c;
    (void)address;
    (void)data;

    return (int)size;
}

int bsp_i2c_read(const bsp_i2c_t *i2c, uint8_t address, uint8_t *data, uint16_t size)
{
    if((i2c == 0) || ((data == 0) && (size > 0U))) {
        return -1;
    }

    (void)i2c;
    (void)address;

    for(uint16_t index = 0U; index < size; index++) {
        data[index] = 0U;
    }

    return (int)size;
}

int bsp_i2c_mem_write(const bsp_i2c_t *i2c, uint8_t address, uint16_t memory_address,
                      const uint8_t *data, uint16_t size)
{
    (void)memory_address;

    return bsp_i2c_write(i2c, address, data, size);
}

int bsp_i2c_mem_read(const bsp_i2c_t *i2c, uint8_t address, uint16_t memory_address,
                     uint8_t *data, uint16_t size)
{
    (void)memory_address;

    return bsp_i2c_read(i2c, address, data, size);
}
