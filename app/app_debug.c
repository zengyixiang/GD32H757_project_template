#include "app_debug.h"

#include "board_uart.h"
#include "log.h"

#include <stddef.h>

static void app_debug_write(const char *data, size_t size)
{
    board_uart_write_buffer(data, (int)size);
}

void app_debug_init(void)
{
    log_set_output(app_debug_write);
    (void)log_init();
}
