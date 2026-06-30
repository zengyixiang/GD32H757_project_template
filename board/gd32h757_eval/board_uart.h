#ifndef BOARD_UART_H
#define BOARD_UART_H

void board_uart_init(void);
void board_uart_write(const char *text);
void board_uart_write_buffer(const char *data, int size);
void board_uart_panic_write_buffer(const char *data, int size);
int board_uart_read_byte(char *data);
int board_uart_wait_byte(char *data);
void board_uart_lock_tx(void);
void board_uart_unlock_tx(void);
void board_uart_irq_handler(void);
void board_uart_dma_irq_handler(void);

#endif
