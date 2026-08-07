#include "board_power.h"

#include "bsp_gpio.h"

static bsp_gpio_t board_sensor_3v3_power;
static bsp_gpio_t board_digital_power;
static bsp_gpio_t board_temp_power;
static bsp_gpio_t board_sys_power;

static const bsp_gpio_config_t board_sensor_3v3_power_config = {
    .clock = RCU_GPIOG,
    .port = GPIOG,
    .pin = GPIO_PIN_1,
    .mode = GPIO_MODE_OUTPUT,
    .pupd = GPIO_PUPD_NONE,
    .output_type = GPIO_OTYPE_PP,
    .speed = GPIO_OSPEED_60MHZ,
};

static const bsp_gpio_config_t board_digital_power_config = {
    .clock = RCU_GPIOC,
    .port = GPIOC,
    .pin = GPIO_PIN_5,
    .mode = GPIO_MODE_OUTPUT,
    .pupd = GPIO_PUPD_NONE,
    .output_type = GPIO_OTYPE_PP,
    .speed = GPIO_OSPEED_60MHZ,
};

static const bsp_gpio_config_t board_temp_power_config = {
    .clock = RCU_GPIOB,
    .port = GPIOB,
    .pin = GPIO_PIN_1,
    .mode = GPIO_MODE_OUTPUT,
    .pupd = GPIO_PUPD_NONE,
    .output_type = GPIO_OTYPE_PP,
    .speed = GPIO_OSPEED_60MHZ,
};

static const bsp_gpio_config_t board_sys_power_config = {
    .clock = RCU_GPIOF,
    .port = GPIOF,
    .pin = GPIO_PIN_15,
    .mode = GPIO_MODE_OUTPUT,
    .pupd = GPIO_PUPD_NONE,
    .output_type = GPIO_OTYPE_PP,
    .speed = GPIO_OSPEED_60MHZ,
};

void board_power_init(void)
{
    bsp_gpio_output_init(&board_sensor_3v3_power, &board_sensor_3v3_power_config);
    bsp_gpio_output_init(&board_digital_power, &board_digital_power_config);
    bsp_gpio_output_init(&board_temp_power, &board_temp_power_config);

    board_power_sensor_all_set(true);
}

void board_power_early_init(void)
{
    /* 先预装载高电平再切换为输出，避免电源锁存脚出现低脉冲。 */
    bsp_gpio_output_init_level(&board_sys_power, &board_sys_power_config, true);
}

void board_power_sensor_3v3_set(bool on)
{
    bsp_gpio_write(&board_sensor_3v3_power, on);
}

void board_power_digital_set(bool on)
{
    bsp_gpio_write(&board_digital_power, !on);
}

void board_power_temp_set(bool on)
{
    bsp_gpio_write(&board_temp_power, !on);
}

void board_power_sensor_all_set(bool on)
{
    board_power_sensor_3v3_set(on);
    board_power_digital_set(on);
    board_power_temp_set(on);
}

void board_power_system_set(bool on)
{
    bsp_gpio_write(&board_sys_power, on);
}
