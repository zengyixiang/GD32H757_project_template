#include "bsp_led.h"

void bsp_led_init(bsp_led_t *led, const bsp_led_config_t *config)
{
    if((led == 0) || (config == 0)) {
        return;
    }

    led->config = *config;
    rcu_periph_clock_enable(led->config.clock);
    gpio_mode_set(led->config.port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, led->config.pin);
    gpio_output_options_set(led->config.port, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, led->config.pin);
}

void bsp_led_set(const bsp_led_t *led, bool on)
{
    if(led == 0) {
        return;
    }

    if(on) {
        gpio_bit_set(led->config.port, led->config.pin);
    } else {
        gpio_bit_reset(led->config.port, led->config.pin);
    }
}

void bsp_led_toggle(const bsp_led_t *led)
{
    if(led == 0) {
        return;
    }

    gpio_bit_toggle(led->config.port, led->config.pin);
}
