#include "bsp_gpio.h"

static void bsp_gpio_config_save(bsp_gpio_t *gpio, const bsp_gpio_config_t *config)
{
    if((gpio == 0) || (config == 0)) {
        return;
    }

    gpio->config = *config;
}

static void bsp_gpio_output_configure(bsp_gpio_t *gpio,
                                      const bsp_gpio_config_t *config,
                                      bool set_initial_level,
                                      bool initial_high)
{
    if((gpio == 0) || (config == 0)) {
        return;
    }

    bsp_gpio_config_save(gpio, config);
    rcu_periph_clock_enable(gpio->config.clock);
    if(set_initial_level) {
        if(initial_high) {
            gpio_bit_set(gpio->config.port, gpio->config.pin);
        } else {
            gpio_bit_reset(gpio->config.port, gpio->config.pin);
        }
    }
    gpio_output_options_set(gpio->config.port,
                            gpio->config.output_type,
                            gpio->config.speed,
                            gpio->config.pin);
    gpio_mode_set(gpio->config.port, gpio->config.mode, gpio->config.pupd, gpio->config.pin);
}

void bsp_gpio_output_init(bsp_gpio_t *gpio, const bsp_gpio_config_t *config)
{
    bsp_gpio_output_configure(gpio, config, false, false);
}

void bsp_gpio_output_init_level(bsp_gpio_t *gpio,
                                const bsp_gpio_config_t *config,
                                bool initial_high)
{
    bsp_gpio_output_configure(gpio, config, true, initial_high);
}

void bsp_gpio_input_init(bsp_gpio_t *gpio, const bsp_gpio_config_t *config)
{
    if((gpio == 0) || (config == 0)) {
        return;
    }

    bsp_gpio_config_save(gpio, config);
    rcu_periph_clock_enable(gpio->config.clock);
    gpio_mode_set(gpio->config.port, gpio->config.mode, gpio->config.pupd, gpio->config.pin);
}

void bsp_gpio_write(const bsp_gpio_t *gpio, bool high)
{
    if(gpio == 0) {
        return;
    }

    if(high) {
        gpio_bit_set(gpio->config.port, gpio->config.pin);
    } else {
        gpio_bit_reset(gpio->config.port, gpio->config.pin);
    }
}

uint8_t bsp_gpio_read(const bsp_gpio_t *gpio)
{
    if(gpio == 0) {
        return 0U;
    }

    return gpio_input_bit_get(gpio->config.port, gpio->config.pin) == SET ? 1U : 0U;
}

void bsp_gpio_exti_init(bsp_gpio_exti_t *exti, const bsp_gpio_exti_config_t *config)
{
    bsp_gpio_t gpio;

    if((exti == 0) || (config == 0)) {
        return;
    }

    exti->config = *config;
    bsp_gpio_input_init(&gpio, &exti->config.gpio);
    rcu_periph_clock_enable(exti->config.syscfg_clock);
    syscfg_exti_line_config(exti->config.exti_port_source, exti->config.exti_pin_source);
    exti_init(exti->config.exti_line, EXTI_INTERRUPT, exti->config.trigger);
    exti_interrupt_flag_clear(exti->config.exti_line);
    exti_interrupt_enable(exti->config.exti_line);
    nvic_irq_enable(exti->config.irqn,
                    exti->config.irq_pre_priority,
                    exti->config.irq_sub_priority);
}

uint8_t bsp_gpio_exti_irq_take(const bsp_gpio_exti_t *exti)
{
    if(exti == 0) {
        return 0U;
    }

    if(RESET == exti_interrupt_flag_get(exti->config.exti_line)) {
        return 0U;
    }

    exti_interrupt_flag_clear(exti->config.exti_line);
    return 1U;
}
