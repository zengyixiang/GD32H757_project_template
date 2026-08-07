#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "gd32h7xx.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    rcu_periph_enum clock;
    uint32_t port;
    uint32_t pin;
    uint32_t mode;
    uint32_t pupd;
    uint8_t output_type;
    uint32_t speed;
} bsp_gpio_config_t;

typedef struct {
    bsp_gpio_config_t config;
} bsp_gpio_t;

typedef struct {
    bsp_gpio_config_t gpio;
    rcu_periph_enum syscfg_clock;
    uint8_t exti_port_source;
    uint8_t exti_pin_source;
    exti_line_enum exti_line;
    exti_trig_type_enum trigger;
    IRQn_Type irqn;
    uint8_t irq_pre_priority;
    uint8_t irq_sub_priority;
} bsp_gpio_exti_config_t;

typedef struct {
    bsp_gpio_exti_config_t config;
} bsp_gpio_exti_t;

void bsp_gpio_output_init(bsp_gpio_t *gpio, const bsp_gpio_config_t *config);
void bsp_gpio_output_init_level(bsp_gpio_t *gpio,
                                const bsp_gpio_config_t *config,
                                bool initial_high);
void bsp_gpio_input_init(bsp_gpio_t *gpio, const bsp_gpio_config_t *config);
void bsp_gpio_write(const bsp_gpio_t *gpio, bool high);
uint8_t bsp_gpio_read(const bsp_gpio_t *gpio);

void bsp_gpio_exti_init(bsp_gpio_exti_t *exti, const bsp_gpio_exti_config_t *config);
uint8_t bsp_gpio_exti_irq_take(const bsp_gpio_exti_t *exti);

#endif
