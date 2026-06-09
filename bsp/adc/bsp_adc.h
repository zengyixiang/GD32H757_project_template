#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "gd32h7xx.h"

#include <stdint.h>

typedef struct {
    uint32_t adc_periph;
    rcu_periph_enum clock;
    uint8_t channel;
} bsp_adc_config_t;

typedef struct {
    bsp_adc_config_t config;
} bsp_adc_t;

void bsp_adc_init(bsp_adc_t *adc, const bsp_adc_config_t *config);
uint16_t bsp_adc_read(const bsp_adc_t *adc);

#endif
