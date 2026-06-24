#ifndef BSP_UART_H
#define BSP_UART_H

#include "FreeRTOS.h"
#include "gd32h7xx.h"
#include "semphr.h"
#include "stream_buffer.h"

#include <stdint.h>

#ifndef BSP_UART_RX_STREAM_BUFFER_SIZE
#define BSP_UART_RX_STREAM_BUFFER_SIZE 256U
#endif

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
    uint8_t enable_tx_dma;
    uint32_t tx_dma_periph;
    dma_channel_enum tx_dma_channel;
    uint32_t tx_dma_request;
    IRQn_Type tx_dma_irq;
    uint8_t tx_dma_pre_priority;
    uint8_t tx_dma_sub_priority;
} bsp_uart_config_t;

typedef struct {
    bsp_uart_config_t config;
    StreamBufferHandle_t rx_stream;
    StaticStreamBuffer_t rx_stream_control;
    uint8_t rx_stream_storage[BSP_UART_RX_STREAM_BUFFER_SIZE];
    SemaphoreHandle_t tx_mutex;
    StaticSemaphore_t tx_mutex_control;
    SemaphoreHandle_t tx_dma_done;
    StaticSemaphore_t tx_dma_done_control;
} bsp_uart_t;

void bsp_uart_init(bsp_uart_t *uart, const bsp_uart_config_t *config);
void bsp_uart_write(bsp_uart_t *uart, const char *text);
void bsp_uart_write_buffer(bsp_uart_t *uart, const char *data, int size);
int bsp_uart_read_byte(const bsp_uart_t *uart, char *data);
int bsp_uart_read_byte_timeout(const bsp_uart_t *uart, char *data, TickType_t ticks_to_wait);
void bsp_uart_lock_tx(const bsp_uart_t *uart);
void bsp_uart_unlock_tx(const bsp_uart_t *uart);
void bsp_uart_enable_rx_interrupt(const bsp_uart_t *uart,
                                  IRQn_Type irq,
                                  uint8_t pre_priority,
                                  uint8_t sub_priority);
void bsp_uart_irq_handler(bsp_uart_t *uart);
void bsp_uart_dma_irq_handler(bsp_uart_t *uart);

#endif
