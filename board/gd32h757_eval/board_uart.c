#include "board_uart.h"

#include "bsp_uart.h"

static bsp_uart_t board_debug_uart;

static const bsp_uart_config_t board_debug_uart_config = {
    .usart_periph = USART0,
    .clock = RCU_USART0,
    .baudrate = 115200U,
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
