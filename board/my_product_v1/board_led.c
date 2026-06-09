#include "board_led.h"

#include "board_hw_version.h"
#include "bsp_led.h"

static bsp_led_t board_leds[BOARD_LED_COUNT];

static const bsp_led_config_t board_led_hw1_configs[BOARD_LED_COUNT] = {
    [BOARD_LED_STATUS] = {
        .clock = RCU_GPIOA,
        .port = GPIOA,
        .pin = GPIO_PIN_2,
    },
};

static const bsp_led_config_t board_led_hw2_configs[BOARD_LED_COUNT] = {
    [BOARD_LED_STATUS] = {
        .clock = RCU_GPIOA,
        .port = GPIOA,
        .pin = GPIO_PIN_3,
    },
};

static const bsp_led_config_t *board_led_configs_get(void)
{
    switch(board_hw_version_get()) {
    case 2U:
        return board_led_hw2_configs;
    case 1U:
    case BOARD_HW_VERSION_UNKNOWN:
    default:
        return board_led_hw1_configs;
    }
}

void board_led_init(void)
{
    const bsp_led_config_t *configs = board_led_configs_get();

    for(board_led_id_t id = BOARD_LED_STATUS; id < BOARD_LED_COUNT; id++) {
        bsp_led_init(&board_leds[id], &configs[id]);
    }
}

void board_led_set(board_led_id_t id, bool on)
{
    if(id >= BOARD_LED_COUNT) {
        return;
    }

    bsp_led_set(&board_leds[id], on);
}

void board_led_toggle(board_led_id_t id)
{
    if(id >= BOARD_LED_COUNT) {
        return;
    }

    bsp_led_toggle(&board_leds[id]);
}
