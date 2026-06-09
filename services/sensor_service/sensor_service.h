#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <stdint.h>

typedef struct {
    int32_t temperature_mc;
    uint32_t humidity_milli_percent;
} sensor_service_environment_t;

void sensor_service_init(void);
uint16_t sensor_service_read_vbat(void);
int sensor_service_read_environment(sensor_service_environment_t *environment);

#endif
