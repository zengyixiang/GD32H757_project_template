#include "hardware_version.h"

#include <stddef.h>

#ifndef HARDWARE_VERSION_MAX_SAMPLES
#define HARDWARE_VERSION_MAX_SAMPLES 16U
#endif

static uint16_t hardware_version_average_u16(const uint16_t *data, size_t count)
{
    uint32_t sum = 0;

    if((data == 0) || (count == 0U)) {
        return 0U;
    }

    for(size_t i = 0; i < count; i++) {
        sum += data[i];
    }

    return (uint16_t)(sum / count);
}

int hardware_version_init(hardware_version_t *version,
                          const hardware_version_config_t *config)
{
    if((version == 0) || (config == 0) || (config->read_mv == 0)) {
        return HARDWARE_VERSION_INVALID_ARG;
    }

    version->config = *config;
    if(version->config.sample_count == 0U) {
        version->config.sample_count = 1U;
    }
    if(version->config.sample_count > HARDWARE_VERSION_MAX_SAMPLES) {
        version->config.sample_count = HARDWARE_VERSION_MAX_SAMPLES;
    }

    version->code = HARDWARE_VERSION_UNKNOWN_CODE;
    version->valid = 0U;

    return HARDWARE_VERSION_OK;
}

hardware_version_code_t hardware_version_code_from_mv(uint16_t mv)
{
    hardware_version_code_t code;

    if(mv <= HARDWARE_VERSION_MIN_MV) {
        return HARDWARE_VERSION_MIN_CODE;
    }
    if(mv >= HARDWARE_VERSION_MAX_MV) {
        return HARDWARE_VERSION_MAX_CODE;
    }

    code = (hardware_version_code_t)(mv / HARDWARE_VERSION_STEP_MV) + 1U;

    if(code < HARDWARE_VERSION_MIN_CODE) {
        code = HARDWARE_VERSION_MIN_CODE;
    }
    if(code > HARDWARE_VERSION_MAX_CODE) {
        code = HARDWARE_VERSION_MAX_CODE;
    }

    return code;
}

int hardware_version_detect(hardware_version_t *version)
{
    uint16_t samples_mv[HARDWARE_VERSION_MAX_SAMPLES];
    uint8_t sample_count;
    uint16_t average_mv;

    if((version == 0) || (version->config.read_mv == 0)) {
        return HARDWARE_VERSION_INVALID_ARG;
    }

    sample_count = version->config.sample_count;
    if(sample_count == 0U) {
        sample_count = 1U;
    }
    if(sample_count > HARDWARE_VERSION_MAX_SAMPLES) {
        sample_count = HARDWARE_VERSION_MAX_SAMPLES;
    }

    for(uint8_t i = 0U; i < sample_count; i++) {
        if(version->config.read_mv(version->config.context, &samples_mv[i]) != 0) {
            version->code = HARDWARE_VERSION_UNKNOWN_CODE;
            version->valid = 0U;
            return HARDWARE_VERSION_READ_FAILED;
        }
    }

    average_mv = hardware_version_average_u16(samples_mv, sample_count);
    version->code = hardware_version_code_from_mv(average_mv);
    version->valid = 1U;

    return HARDWARE_VERSION_OK;
}

hardware_version_code_t hardware_version_code(const hardware_version_t *version)
{
    if((version == 0) || (version->valid == 0U)) {
        return HARDWARE_VERSION_UNKNOWN_CODE;
    }

    return version->code;
}

uint8_t hardware_version_is_valid(const hardware_version_t *version)
{
    if(version == 0) {
        return 0U;
    }

    return version->valid;
}
