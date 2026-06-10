#include "bsp_uart.h"

static void bsp_uart_pin_init(const bsp_uart_pin_config_t *pin)
{
    rcu_periph_clock_enable(pin->clock);
    gpio_af_set(pin->port, pin->alternate, pin->pin);
    gpio_mode_set(pin->port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin->pin);
    gpio_output_options_set(pin->port, GPIO_OTYPE_PP, GPIO_OSPEED_12MHZ, pin->pin);
}

void bsp_uart_init(bsp_uart_t *uart, const bsp_uart_config_t *config)
{
    if((uart == 0) || (config == 0)) {
        return;
    }

    uart->config = *config;
    bsp_uart_pin_init(&uart->config.tx);
    if(uart->config.enable_rx != 0U) {
        bsp_uart_pin_init(&uart->config.rx);
    }

    rcu_periph_clock_enable(uart->config.clock);
    usart_deinit(uart->config.usart_periph);
    usart_baudrate_set(uart->config.usart_periph, uart->config.baudrate);
    usart_stop_bit_set(uart->config.usart_periph, USART_STB_1BIT);
    usart_word_length_set(uart->config.usart_periph, USART_WL_8BIT);
    usart_parity_config(uart->config.usart_periph, USART_PM_NONE);
    usart_transmit_config(uart->config.usart_periph, USART_TRANSMIT_ENABLE);
    if(uart->config.enable_rx != 0U) {
        usart_receive_config(uart->config.usart_periph, USART_RECEIVE_ENABLE);
    }
    usart_enable(uart->config.usart_periph);
}

void bsp_uart_write(const bsp_uart_t *uart, const char *text)
{
    int size = 0;

    if((uart == 0) || (text == 0)) {
        return;
    }

    while(text[size] != '\0') {
        size++;
    }

    bsp_uart_write_buffer(uart, text, size);
}

void bsp_uart_write_buffer(const bsp_uart_t *uart, const char *data, int size)
{
    int i;

    if((uart == 0) || (data == 0) || (size <= 0)) {
        return;
    }

    for(i = 0; i < size; i++) {
        while(RESET == usart_flag_get(uart->config.usart_periph, USART_FLAG_TBE)) {
        }
        usart_data_transmit(uart->config.usart_periph, (uint8_t)data[i]);
    }

    while(RESET == usart_flag_get(uart->config.usart_periph, USART_FLAG_TC)) {
    }
}
