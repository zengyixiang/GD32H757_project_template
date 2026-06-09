#ifndef BOARD_SENSOR_H
#define BOARD_SENSOR_H

#include <stdint.h>

typedef struct {
    int32_t temperature_mc;
    uint32_t humidity_milli_percent;
} board_environment_sample_t;

void board_sensor_init(void);
int board_sensor_read_environment(board_environment_sample_t *sample);

#endif
