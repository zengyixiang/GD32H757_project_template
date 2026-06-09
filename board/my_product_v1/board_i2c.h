#ifndef BOARD_I2C_H
#define BOARD_I2C_H

#include <stdint.h>

typedef enum {
    BOARD_I2C_EEPROM = 0,
    BOARD_I2C_SENSOR,
    BOARD_I2C_COUNT
} board_i2c_id_t;

void board_i2c_init(void);
int board_i2c_write(board_i2c_id_t id, uint8_t address, const uint8_t *data, uint16_t size);
int board_i2c_read(board_i2c_id_t id, uint8_t address, uint8_t *data, uint16_t size);
int board_i2c_mem_write(board_i2c_id_t id, uint8_t address, uint16_t memory_address,
                        const uint8_t *data, uint16_t size);
int board_i2c_mem_read(board_i2c_id_t id, uint8_t address, uint16_t memory_address,
                       uint8_t *data, uint16_t size);

#endif
