#include "board_uart.h"

#include "bsp_uart.h"

static bsp_uart_t board_debug_uart;
static uint8_t board_debug_uart_initialized;

static const bsp_uart_config_t board_debug_uart_config = {
    .usart_periph = USART0,
    .clock = RCU_USART0,
    .baudrate = 115200U,
    .tx = {
        .port = GPIOA,
        .pin = GPIO_PIN_9,
        .alternate = GPIO_AF_7,
        .clock = RCU_GPIOA,
    },
    .rx = {
        .port = GPIOA,
        .pin = GPIO_PIN_10,
        .alternate = GPIO_AF_7,
        .clock = RCU_GPIOA,
    },
    .enable_rx = 1U,
    .enable_tx_dma = 1U,
    .tx_dma_periph = DMA1,
    .tx_dma_channel = DMA_CH7,
    .tx_dma_request = DMA_REQUEST_USART0_TX,
    .tx_dma_irq = DMA1_Channel7_IRQn,
    .tx_dma_pre_priority = 6U,
    .tx_dma_sub_priority = 0U,
};

void board_uart_init(void)
{
    bsp_uart_init(&board_debug_uart, &board_debug_uart_config);
    bsp_uart_enable_rx_interrupt(&board_debug_uart, USART0_IRQn, 6U, 0U);
    board_debug_uart_initialized = 1U;
}

void board_uart_write(const char *text)
{
    bsp_uart_write(&board_debug_uart, text);
}

void board_uart_write_buffer(const char *data, int size)
{
    bsp_uart_write_buffer(&board_debug_uart, data, size);
}

void board_uart_panic_write_buffer(const char *data, int size)
{
    if(board_debug_uart_initialized == 0U) {
        bsp_uart_panic_init(&board_debug_uart, &board_debug_uart_config);
        board_debug_uart_initialized = 1U;
    }

    bsp_uart_panic_write_buffer(&board_debug_uart, data, size);
}

int board_uart_read_byte(char *data)
{
    return bsp_uart_read_byte(&board_debug_uart, data);
}

int board_uart_wait_byte(char *data)
{
    return bsp_uart_read_byte_timeout(&board_debug_uart, data, portMAX_DELAY);
}

void board_uart_lock_tx(void)
{
    bsp_uart_lock_tx(&board_debug_uart);
}

void board_uart_unlock_tx(void)
{
    bsp_uart_unlock_tx(&board_debug_uart);
}

void board_uart_irq_handler(void)
{
    bsp_uart_irq_handler(&board_debug_uart);
}

void board_uart_dma_irq_handler(void)
{
    bsp_uart_dma_irq_handler(&board_debug_uart);
}
