#ifndef HARDWARE_VERSION_H
#define HARDWARE_VERSION_H

#include <stdint.h>

#define HARDWARE_VERSION_MIN_MV 0U
#define HARDWARE_VERSION_MAX_MV 3000U
#define HARDWARE_VERSION_STEP_MV 100U

#define HARDWARE_VERSION_MIN_CODE 1U
#define HARDWARE_VERSION_MAX_CODE 30U
#define HARDWARE_VERSION_UNKNOWN_CODE 0U

#define HARDWARE_VERSION_OK 0
#define HARDWARE_VERSION_INVALID_ARG (-1)
#define HARDWARE_VERSION_READ_FAILED (-2)

typedef uint8_t hardware_version_code_t;

typedef int (*hardware_version_read_mv_fn)(void *context, uint16_t *mv);

typedef struct {
    hardware_version_read_mv_fn read_mv;
    void *context;
    uint8_t sample_count;
} hardware_version_config_t;

typedef struct {
    hardware_version_config_t config;
    hardware_version_code_t code;
    uint8_t valid;
} hardware_version_t;

int hardware_version_init(hardware_version_t *version,
                          const hardware_version_config_t *config);
int hardware_version_detect(hardware_version_t *version);

hardware_version_code_t hardware_version_code_from_mv(uint16_t mv);
hardware_version_code_t hardware_version_code(const hardware_version_t *version);
uint8_t hardware_version_is_valid(const hardware_version_t *version);

#endif
