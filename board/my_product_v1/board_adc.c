#include "board_adc.h"

#include "bsp_adc.h"

static bsp_adc_t board_adcs[BOARD_ADC_COUNT];

static const bsp_adc_config_t board_adc_configs[BOARD_ADC_COUNT] = {
    [BOARD_ADC_VBAT] = {
        .adc_periph = ADC0,
        .clock = RCU_ADC0,
        .channel = 5U,
    },
    [BOARD_ADC_TEMP] = {
        .adc_periph = ADC1,
        .clock = RCU_ADC1,
        .channel = 8U,
    },
};

void board_adc_init(void)
{
    for(board_adc_id_t id = BOARD_ADC_VBAT; id < BOARD_ADC_COUNT; id++) {
        bsp_adc_init(&board_adcs[id], &board_adc_configs[id]);
    }
}

uint16_t board_adc_read(board_adc_id_t id)
{
    if(id >= BOARD_ADC_COUNT) {
        return 0U;
    }

    return bsp_adc_read(&board_adcs[id]);
}
