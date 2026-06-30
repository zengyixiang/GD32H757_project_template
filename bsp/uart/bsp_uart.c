#include "bsp_uart.h"

#include "task.h"

#define BSP_UART_DMA_MAX_TRANSFER_SIZE 65535U
#define BSP_UART_DCACHE_LINE_SIZE 32U
#define BSP_UART_DMA_ALL_FLAGS (DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF)

static void bsp_uart_pin_init(const bsp_uart_pin_config_t *pin)
{
    rcu_periph_clock_enable(pin->clock);
    gpio_af_set(pin->port, pin->alternate, pin->pin);
    gpio_mode_set(pin->port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin->pin);
    gpio_output_options_set(pin->port, GPIO_OTYPE_PP, GPIO_OSPEED_12MHZ, pin->pin);
}

static void bsp_uart_wait_transfer_complete(uint32_t usart_periph)
{
    while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC)) {
    }
}

static void bsp_uart_write_buffer_polling(const bsp_uart_t *uart, const char *data, uint32_t size)
{
    uint32_t i;

    for(i = 0; i < size; i++) {
        while(RESET == usart_flag_get(uart->config.usart_periph, USART_FLAG_TBE)) {
        }
        usart_data_transmit(uart->config.usart_periph, (uint8_t)data[i]);
    }

    bsp_uart_wait_transfer_complete(uart->config.usart_periph);
}

static void bsp_uart_dma_clock_enable(uint32_t dma_periph)
{
    if(dma_periph == DMA0) {
        rcu_periph_clock_enable(RCU_DMA0);
    } else if(dma_periph == DMA1) {
        rcu_periph_clock_enable(RCU_DMA1);
    }

    rcu_periph_clock_enable(RCU_DMAMUX);
}

static void bsp_uart_clean_dcache(const void *data, uint32_t size)
{
    uintptr_t start;
    uintptr_t end;

    if((data == 0) || (size == 0U)) {
        return;
    }

    start = (uintptr_t)data;
    end = start + (uintptr_t)size;
    start &= ~((uintptr_t)BSP_UART_DCACHE_LINE_SIZE - 1U);
    end = (end + ((uintptr_t)BSP_UART_DCACHE_LINE_SIZE - 1U)) &
          ~((uintptr_t)BSP_UART_DCACHE_LINE_SIZE - 1U);

    SCB_CleanDCache_by_Addr((void *)start, (int32_t)(end - start));
}

static void bsp_uart_tx_dma_init(bsp_uart_t *uart)
{
    dma_single_data_parameter_struct dma_init_struct;

    if((uart->config.enable_tx_dma == 0U) || (uart->tx_dma_done == NULL)) {
        return;
    }

    bsp_uart_dma_clock_enable(uart->config.tx_dma_periph);

    dma_deinit(uart->config.tx_dma_periph, uart->config.tx_dma_channel);
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.request = uart->config.tx_dma_request;
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr = 0U;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number = 0U;
    dma_init_struct.periph_addr = (uint32_t)&USART_TDATA(uart->config.usart_periph);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(uart->config.tx_dma_periph,
                              uart->config.tx_dma_channel,
                              &dma_init_struct);
    dma_circulation_disable(uart->config.tx_dma_periph, uart->config.tx_dma_channel);
    dma_flag_clear(uart->config.tx_dma_periph,
                   uart->config.tx_dma_channel,
                   BSP_UART_DMA_ALL_FLAGS);
    dma_interrupt_enable(uart->config.tx_dma_periph,
                         uart->config.tx_dma_channel,
                         DMA_INT_FTF);

    usart_dma_transmit_config(uart->config.usart_periph, USART_TRANSMIT_DMA_ENABLE);
    nvic_irq_enable(uart->config.tx_dma_irq,
                    uart->config.tx_dma_pre_priority,
                    uart->config.tx_dma_sub_priority);
}

static BaseType_t bsp_uart_write_dma_chunk(bsp_uart_t *uart, const char *data, uint32_t size)
{
    while(xSemaphoreTake(uart->tx_dma_done, 0U) == pdTRUE) {
    }

    dma_channel_disable(uart->config.tx_dma_periph, uart->config.tx_dma_channel);
    dma_flag_clear(uart->config.tx_dma_periph,
                   uart->config.tx_dma_channel,
                   BSP_UART_DMA_ALL_FLAGS);
    dma_periph_address_config(uart->config.tx_dma_periph,
                              uart->config.tx_dma_channel,
                              (uint32_t)&USART_TDATA(uart->config.usart_periph));
    dma_memory_address_config(uart->config.tx_dma_periph,
                              uart->config.tx_dma_channel,
                              DMA_MEMORY_0,
                              (uint32_t)data);
    dma_transfer_number_config(uart->config.tx_dma_periph, uart->config.tx_dma_channel, size);
    bsp_uart_clean_dcache(data, size);
    usart_flag_clear(uart->config.usart_periph, USART_FLAG_TC);
    dma_channel_enable(uart->config.tx_dma_periph, uart->config.tx_dma_channel);

    if(xSemaphoreTake(uart->tx_dma_done, portMAX_DELAY) != pdTRUE) {
        return pdFALSE;
    }

    return pdTRUE;
}

static BaseType_t bsp_uart_write_buffer_dma(bsp_uart_t *uart, const char *data, uint32_t size)
{
    const char *cursor = data;
    uint32_t remaining = size;

    while(remaining > 0U) {
        uint32_t chunk_size = remaining;

        if(chunk_size > BSP_UART_DMA_MAX_TRANSFER_SIZE) {
            chunk_size = BSP_UART_DMA_MAX_TRANSFER_SIZE;
        }

        if(bsp_uart_write_dma_chunk(uart, cursor, chunk_size) != pdTRUE) {
            return pdFALSE;
        }

        cursor += chunk_size;
        remaining -= chunk_size;
    }

    bsp_uart_wait_transfer_complete(uart->config.usart_periph);
    return pdTRUE;
}

void bsp_uart_init(bsp_uart_t *uart, const bsp_uart_config_t *config)
{
    if((uart == 0) || (config == 0)) {
        return;
    }

    uart->config = *config;
    uart->tx_mutex = xSemaphoreCreateRecursiveMutexStatic(&uart->tx_mutex_control);
    uart->tx_dma_done = NULL;
    bsp_uart_pin_init(&uart->config.tx);
    if(uart->config.enable_rx != 0U) {
        bsp_uart_pin_init(&uart->config.rx);
        uart->rx_stream = xStreamBufferCreateStatic(sizeof(uart->rx_stream_storage),
                                                    1U,
                                                    uart->rx_stream_storage,
                                                    &uart->rx_stream_control);
    } else {
        uart->rx_stream = NULL;
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

    if(uart->config.enable_tx_dma != 0U) {
        uart->tx_dma_done = xSemaphoreCreateBinaryStatic(&uart->tx_dma_done_control);
        bsp_uart_tx_dma_init(uart);
    }
}

void bsp_uart_panic_init(bsp_uart_t *uart, const bsp_uart_config_t *config)
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

void bsp_uart_write(bsp_uart_t *uart, const char *text)
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

void bsp_uart_write_buffer(bsp_uart_t *uart, const char *data, int size)
{
    BaseType_t locked = pdFALSE;
    BaseType_t use_dma = pdFALSE;

    if((uart == 0) || (data == 0) || (size <= 0)) {
        return;
    }

    if((uart->tx_mutex != NULL) && (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)) {
        locked = xSemaphoreTakeRecursive(uart->tx_mutex, portMAX_DELAY);
    }

    if((uart->config.enable_tx_dma != 0U) &&
       (uart->tx_dma_done != NULL) &&
       (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)) {
        use_dma = pdTRUE;
    }

    if(use_dma == pdTRUE) {
        (void)bsp_uart_write_buffer_dma(uart, data, (uint32_t)size);
    } else {
        bsp_uart_write_buffer_polling(uart, data, (uint32_t)size);
    }

    if(locked == pdTRUE) {
        (void)xSemaphoreGiveRecursive(uart->tx_mutex);
    }
}

void bsp_uart_panic_write_buffer(const bsp_uart_t *uart, const char *data, int size)
{
    if((uart == 0) || (data == 0) || (size <= 0)) {
        return;
    }

    bsp_uart_write_buffer_polling(uart, data, (uint32_t)size);
}

int bsp_uart_read_byte(const bsp_uart_t *uart, char *data)
{
    return bsp_uart_read_byte_timeout(uart, data, 0U);
}

int bsp_uart_read_byte_timeout(const bsp_uart_t *uart, char *data, TickType_t ticks_to_wait)
{
    if((uart == 0) || (data == 0) || (uart->rx_stream == NULL)) {
        return 0;
    }

    return (xStreamBufferReceive(uart->rx_stream, data, 1U, ticks_to_wait) == 1U) ? 1 : 0;
}

void bsp_uart_lock_tx(const bsp_uart_t *uart)
{
    if((uart == 0) || (uart->tx_mutex == NULL)) {
        return;
    }

    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        (void)xSemaphoreTakeRecursive(uart->tx_mutex, portMAX_DELAY);
    }
}

void bsp_uart_unlock_tx(const bsp_uart_t *uart)
{
    if((uart == 0) || (uart->tx_mutex == NULL)) {
        return;
    }

    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        (void)xSemaphoreGiveRecursive(uart->tx_mutex);
    }
}

void bsp_uart_enable_rx_interrupt(const bsp_uart_t *uart,
                                  IRQn_Type irq,
                                  uint8_t pre_priority,
                                  uint8_t sub_priority)
{
    if((uart == 0) || (uart->config.enable_rx == 0U) || (uart->rx_stream == NULL)) {
        return;
    }

    usart_interrupt_enable(uart->config.usart_periph, USART_INT_RBNE);
    nvic_irq_enable(irq, pre_priority, sub_priority);
}

void bsp_uart_irq_handler(bsp_uart_t *uart)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    if((uart == 0) || (uart->rx_stream == NULL)) {
        return;
    }

    while(usart_flag_get(uart->config.usart_periph, USART_FLAG_RBNE) != RESET) {
        char data = (char)(usart_data_receive(uart->config.usart_periph) & 0xFFU);
        (void)xStreamBufferSendFromISR(uart->rx_stream,
                                       &data,
                                       1U,
                                       &higher_priority_task_woken);
    }

    if(usart_flag_get(uart->config.usart_periph, USART_FLAG_ORERR) != RESET) {
        usart_interrupt_flag_clear(uart->config.usart_periph, USART_INT_FLAG_RBNE_ORERR);
    }

    portYIELD_FROM_ISR(higher_priority_task_woken);
}

void bsp_uart_dma_irq_handler(bsp_uart_t *uart)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    if((uart == 0) || (uart->tx_dma_done == NULL) || (uart->config.enable_tx_dma == 0U)) {
        return;
    }

    if(dma_interrupt_flag_get(uart->config.tx_dma_periph,
                              uart->config.tx_dma_channel,
                              DMA_INT_FLAG_FTF) != RESET) {
        dma_interrupt_flag_clear(uart->config.tx_dma_periph,
                                 uart->config.tx_dma_channel,
                                 DMA_INT_FLAG_FTF);
        dma_channel_disable(uart->config.tx_dma_periph, uart->config.tx_dma_channel);
        (void)xSemaphoreGiveFromISR(uart->tx_dma_done, &higher_priority_task_woken);
    }

    portYIELD_FROM_ISR(higher_priority_task_woken);
}
