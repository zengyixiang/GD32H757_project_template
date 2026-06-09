#ifndef BSP_LED_H
#define BSP_LED_H

#include "gd32h7xx.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    rcu_periph_enum clock;
    uint32_t port;
    uint32_t pin;
} bsp_led_config_t;

typedef struct {
    bsp_led_config_t config;
} bsp_led_t;

void bsp_led_init(bsp_led_t *led, const bsp_led_config_t *config);
void bsp_led_set(const bsp_led_t *led, bool on);
void bsp_led_toggle(const bsp_led_t *led);

#endif
