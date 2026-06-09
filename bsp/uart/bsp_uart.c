#include "bsp_uart.h"

void bsp_uart_init(bsp_uart_t *uart, const bsp_uart_config_t *config)
{
    if((uart == 0) || (config == 0)) {
        return;
    }

    uart->config = *config;
    rcu_periph_clock_enable(uart->config.clock);
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
    if((uart == 0) || (data == 0) || (size <= 0)) {
        return;
    }

    (void)uart;
    (void)data;
    (void)size;
}
