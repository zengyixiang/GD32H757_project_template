#include "sensor_service.h"

#include "board_adc.h"
#include "board_sensor.h"

void sensor_service_init(void)
{
}

uint16_t sensor_service_read_vbat(void)
{
    return board_adc_read(BOARD_ADC_VBAT);
}

int sensor_service_read_environment(sensor_service_environment_t *environment)
{
    board_environment_sample_t board_sample;
    int result;

    if(environment == 0) {
        return -1;
    }

    result = board_sensor_read_environment(&board_sample);
    if(result != 0) {
        return result;
    }

    environment->temperature_mc = board_sample.temperature_mc;
    environment->humidity_milli_percent = board_sample.humidity_milli_percent;

    return 0;
}
