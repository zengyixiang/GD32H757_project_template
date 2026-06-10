#include "board_uart.h"

#include "bsp_uart.h"

static bsp_uart_t board_debug_uart;

static const bsp_uart_config_t board_debug_uart_config = {
    .usart_periph = USART2,
    .clock = RCU_USART2,
    .baudrate = 115200U,
    .tx = {
        .port = GPIOB,
        .pin = GPIO_PIN_10,
        .alternate = GPIO_AF_7,
        .clock = RCU_GPIOB,
    },
    .rx = {
        .port = GPIOB,
        .pin = GPIO_PIN_11,
        .alternate = GPIO_AF_7,
        .clock = RCU_GPIOB,
    },
    .enable_rx = 1U,
};

void board_uart_init(void)
{
    bsp_uart_init(&board_debug_uart, &board_debug_uart_config);
}

void board_uart_write(const char *text)
{
    bsp_uart_write(&board_debug_uart, text);
}

void board_uart_write_buffer(const char *data, int size)
{
    bsp_uart_write_buffer(&board_debug_uart, data, size);
}
