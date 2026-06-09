#include "board_hw_version.h"

#include "bsp_adc.h"
#include "hardware_version.h"

#define BOARD_HW_VERSION_ADC_REF_MV 3000U
#define BOARD_HW_VERSION_ADC_MAX_RAW 4095U

static bsp_adc_t board_hw_version_adc;
static hardware_version_t board_hw_version_detector;

static const bsp_adc_config_t board_hw_version_adc_config = {
    .adc_periph = ADC2,
    .clock = RCU_ADC2,
    .channel = 12U,
};

static uint16_t board_hw_version_adc_to_mv(uint16_t raw_adc)
{
    uint32_t mv = ((uint32_t)raw_adc * BOARD_HW_VERSION_ADC_REF_MV) +
                  (BOARD_HW_VERSION_ADC_MAX_RAW / 2U);

    return (uint16_t)(mv / BOARD_HW_VERSION_ADC_MAX_RAW);
}

static int board_hw_version_sample_mv(void *context, uint16_t *mv)
{
    uint16_t raw_adc;

    (void)context;

    if(mv == 0) {
        return -1;
    }

    raw_adc = bsp_adc_read(&board_hw_version_adc);
    *mv = board_hw_version_adc_to_mv(raw_adc);

    return 0;
}

void board_hw_version_init(void)
{
    const hardware_version_config_t config = {
        .read_mv = board_hw_version_sample_mv,
        .context = 0,
        .sample_count = 8U,
    };

    bsp_adc_init(&board_hw_version_adc, &board_hw_version_adc_config);
    (void)hardware_version_init(&board_hw_version_detector, &config);
    (void)hardware_version_detect(&board_hw_version_detector);
}

board_hw_version_t board_hw_version_get(void)
{
    return hardware_version_code(&board_hw_version_detector);
}

uint8_t board_hw_version_is_valid(void)
{
    return hardware_version_is_valid(&board_hw_version_detector);
}
