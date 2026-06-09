#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "gd32h7xx.h"

#include <stdint.h>

typedef struct {
    uint32_t i2c_periph;
    rcu_periph_enum clock;
    uint32_t speed_hz;
} bsp_i2c_config_t;

typedef struct {
    bsp_i2c_config_t config;
} bsp_i2c_t;

void bsp_i2c_init(bsp_i2c_t *i2c, const bsp_i2c_config_t *config);
int bsp_i2c_write(const bsp_i2c_t *i2c, uint8_t address, const uint8_t *data, uint16_t size);
int bsp_i2c_read(const bsp_i2c_t *i2c, uint8_t address, uint8_t *data, uint16_t size);
int bsp_i2c_mem_write(const bsp_i2c_t *i2c, uint8_t address, uint16_t memory_address,
                      const uint8_t *data, uint16_t size);
int bsp_i2c_mem_read(const bsp_i2c_t *i2c, uint8_t address, uint16_t memory_address,
                     uint8_t *data, uint16_t size);

#endif
