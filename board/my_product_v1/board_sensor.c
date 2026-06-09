#include "board_sensor.h"

#include "board_hw_version.h"
#include "board_i2c.h"
#include "gxht30.h"
#include "osal.h"

#include <stdint.h>

static gxht30_t board_gxht30;

typedef struct {
    uint8_t device_address;
    board_i2c_id_t i2c_id;
} board_gxht30_revision_config_t;

static const board_gxht30_revision_config_t board_gxht30_hw1_config = {
    .device_address = GXHT30_DEFAULT_ADDRESS,
    .i2c_id = BOARD_I2C_SENSOR,
};

static const board_gxht30_revision_config_t board_gxht30_hw2_config = {
    .device_address = 0x45U,
    .i2c_id = BOARD_I2C_SENSOR,
};

static const board_gxht30_revision_config_t *board_gxht30_config_get(void)
{
    switch(board_hw_version_get()) {
    case 2U:
        return &board_gxht30_hw2_config;
    case 1U:
    case BOARD_HW_VERSION_UNKNOWN:
    default:
        return &board_gxht30_hw1_config;
    }
}

static int board_gxht30_write(void *context, uint8_t device_address,
                              const uint8_t *data, uint16_t size)
{
    board_i2c_id_t i2c_id = (board_i2c_id_t)(uintptr_t)context;

    return board_i2c_write(i2c_id, device_address, data, size);
}

static int board_gxht30_read(void *context, uint8_t device_address,
                             uint8_t *data, uint16_t size)
{
    board_i2c_id_t i2c_id = (board_i2c_id_t)(uintptr_t)context;

    return board_i2c_read(i2c_id, device_address, data, size);
}

static void board_gxht30_delay_ms(uint32_t timeout_ms)
{
    osal_delay_ms(timeout_ms);
}

void board_sensor_init(void)
{
    const board_gxht30_revision_config_t *revision_config = board_gxht30_config_get();
    const gxht30_config_t config = {
        .device_address = revision_config->device_address,
        .bus_context = (void *)(uintptr_t)revision_config->i2c_id,
        .write = board_gxht30_write,
        .read = board_gxht30_read,
        .delay_ms = board_gxht30_delay_ms,
    };

    (void)gxht30_init(&board_gxht30, &config);
}

int board_sensor_read_environment(board_environment_sample_t *sample)
{
    gxht30_sample_t gxht30_sample;
    int result;

    if(sample == 0) {
        return -1;
    }

    result = gxht30_read_sample(&board_gxht30, &gxht30_sample);
    if(result != 0) {
        return result;
    }

    sample->temperature_mc = gxht30_sample.temperature_mc;
    sample->humidity_milli_percent = gxht30_sample.humidity_milli_percent;

    return 0;
}
