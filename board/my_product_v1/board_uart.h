#ifndef BOARD_UART_H
#define BOARD_UART_H

void board_uart_init(void);
void board_uart_write(const char *text);
void board_uart_write_buffer(const char *data, int size);

#endif
