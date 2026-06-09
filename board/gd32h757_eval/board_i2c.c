#include "board_i2c.h"

#include "bsp_i2c.h"

static bsp_i2c_t board_i2cs[BOARD_I2C_COUNT];

static const bsp_i2c_config_t board_i2c_configs[BOARD_I2C_COUNT] = {
    [BOARD_I2C_EEPROM] = {
        .i2c_periph = I2C0,
        .clock = RCU_I2C0,
        .speed_hz = 100000U,
    },
    [BOARD_I2C_SENSOR] = {
        .i2c_periph = I2C1,
        .clock = RCU_I2C1,
        .speed_hz = 400000U,
    },
};

void board_i2c_init(void)
{
    for(board_i2c_id_t id = BOARD_I2C_EEPROM; id < BOARD_I2C_COUNT; id++) {
        bsp_i2c_init(&board_i2cs[id], &board_i2c_configs[id]);
    }
}

int board_i2c_write(board_i2c_id_t id, uint8_t address, const uint8_t *data, uint16_t size)
{
    if(id >= BOARD_I2C_COUNT) {
        return -1;
    }

    return bsp_i2c_write(&board_i2cs[id], address, data, size);
}

int board_i2c_read(board_i2c_id_t id, uint8_t address, uint8_t *data, uint16_t size)
{
    if(id >= BOARD_I2C_COUNT) {
        return -1;
    }

    return bsp_i2c_read(&board_i2cs[id], address, data, size);
}

int board_i2c_mem_write(board_i2c_id_t id, uint8_t address, uint16_t memory_address,
                        const uint8_t *data, uint16_t size)
{
    if(id >= BOARD_I2C_COUNT) {
        return -1;
    }

    return bsp_i2c_mem_write(&board_i2cs[id], address, memory_address, data, size);
}

int board_i2c_mem_read(board_i2c_id_t id, uint8_t address, uint16_t memory_address,
                       uint8_t *data, uint16_t size)
{
    if(id >= BOARD_I2C_COUNT) {
        return -1;
    }

    return bsp_i2c_mem_read(&board_i2cs[id], address, memory_address, data, size);
}
