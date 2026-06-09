#include "bsp_adc.h"

void bsp_adc_init(bsp_adc_t *adc, const bsp_adc_config_t *config)
{
    if((adc == 0) || (config == 0)) {
        return;
    }

    adc->config = *config;
    rcu_periph_clock_enable(adc->config.clock);
}

uint16_t bsp_adc_read(const bsp_adc_t *adc)
{
    if(adc == 0) {
        return 0U;
    }

    (void)adc;

    return 0U;
}
