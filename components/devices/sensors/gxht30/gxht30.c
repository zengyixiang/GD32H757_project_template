#include "gxht30.h"

int gxht30_init(gxht30_t *sensor, const gxht30_config_t *config)
{
    if((sensor == 0) || (config == 0) || (config->write == 0) || (config->read == 0)) {
        return -1;
    }

    sensor->config = *config;

    if(sensor->config.device_address == 0U) {
        sensor->config.device_address = GXHT30_DEFAULT_ADDRESS;
    }

    return 0;
}

int gxht30_read_sample(gxht30_t *sensor, gxht30_sample_t *sample)
{
    uint8_t command[2] = {0x2CU, 0x06U};
    uint8_t raw_data[6] = {0U};
    uint16_t raw_temperature;
    uint16_t raw_humidity;

    if((sensor == 0) || (sample == 0)) {
        return -1;
    }

    if(sensor->config.write(sensor->config.bus_context, sensor->config.device_address,
                            command, sizeof(command)) != (int)sizeof(command)) {
        return -2;
    }

    if(sensor->config.delay_ms != 0) {
        sensor->config.delay_ms(20U);
    }

    if(sensor->config.read(sensor->config.bus_context, sensor->config.device_address,
                           raw_data, sizeof(raw_data)) != (int)sizeof(raw_data)) {
        return -3;
    }

    raw_temperature = (uint16_t)((raw_data[0] << 8U) | raw_data[1]);
    raw_humidity = (uint16_t)((raw_data[3] << 8U) | raw_data[4]);

    sample->temperature_mc = (int32_t)(-45000 + ((int64_t)175000 * raw_temperature) / 65535);
    sample->humidity_milli_percent = (uint32_t)(((uint64_t)100000 * raw_humidity) / 65535U);

    return 0;
}
