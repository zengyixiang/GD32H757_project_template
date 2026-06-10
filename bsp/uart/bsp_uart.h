#ifndef BSP_UART_H
#define BSP_UART_H

#include "gd32h7xx.h"

#include <stdint.h>

typedef struct {
    uint32_t port;
    uint32_t pin;
    uint32_t alternate;
    rcu_periph_enum clock;
} bsp_uart_pin_config_t;

typedef struct {
    uint32_t usart_periph;
    rcu_periph_enum clock;
    uint32_t baudrate;
    bsp_uart_pin_config_t tx;
    bsp_uart_pin_config_t rx;
    uint8_t enable_rx;
} bsp_uart_config_t;

typedef struct {
    bsp_uart_config_t config;
} bsp_uart_t;

void bsp_uart_init(bsp_uart_t *uart, const bsp_uart_config_t *config);
void bsp_uart_write(const bsp_uart_t *uart, const char *text);
void bsp_uart_write_buffer(const bsp_uart_t *uart, const char *data, int size);

#endif
