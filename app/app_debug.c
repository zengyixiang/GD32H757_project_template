#include "app_debug.h"

#include "board_uart.h"
#include "debug.h"
#include "osal.h"

static void app_debug_write(char *str, debug_printf_len_t size)
{
    board_uart_write_buffer(str, (int)size);
}

static debug_critical_t app_debug_enter_critical(void)
{
    return (debug_critical_t)osal_enter_critical();
}

static void app_debug_exit_critical(debug_critical_t state)
{
    osal_exit_critical((unsigned int)state);
}

static debug_time_t app_debug_get_time_ms(void)
{
    return (debug_time_t)osal_millis();
}

void app_debug_init(void)
{
    debug_init(app_debug_write,
               app_debug_enter_critical,
               app_debug_exit_critical,
               app_debug_get_time_ms);
}

void app_debug_flush(void)
{
    debug_log_flush();
}
