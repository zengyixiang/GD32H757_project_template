#ifndef GXHT30_H
#define GXHT30_H

#include <stdint.h>

#define GXHT30_DEFAULT_ADDRESS 0x44U

typedef int (*gxht30_i2c_write_fn)(void *context, uint8_t device_address,
                                   const uint8_t *data, uint16_t size);
typedef int (*gxht30_i2c_read_fn)(void *context, uint8_t device_address,
                                  uint8_t *data, uint16_t size);
typedef void (*gxht30_delay_ms_fn)(uint32_t timeout_ms);

typedef struct {
    int32_t temperature_mc;
    uint32_t humidity_milli_percent;
} gxht30_sample_t;

typedef struct {
    uint8_t device_address;
    void *bus_context;
    gxht30_i2c_write_fn write;
    gxht30_i2c_read_fn read;
    gxht30_delay_ms_fn delay_ms;
} gxht30_config_t;

typedef struct {
    gxht30_config_t config;
} gxht30_t;

int gxht30_init(gxht30_t *sensor, const gxht30_config_t *config);
int gxht30_read_sample(gxht30_t *sensor, gxht30_sample_t *sample);

#endif
