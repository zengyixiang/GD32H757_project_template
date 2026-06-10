#include "app_debug.h"

#include "board_uart.h"
#include "debug.h"

#include "FreeRTOS.h"
#include "task.h"

static void app_debug_write(char *str, debug_printf_len_t size)
{
    board_uart_write_buffer(str, (int)size);
}

static debug_critical_t app_debug_enter_critical(void)
{
    taskENTER_CRITICAL();
    return (debug_critical_t)0U;
}

static void app_debug_exit_critical(debug_critical_t state)
{
    (void)state;
    taskEXIT_CRITICAL();
}

static debug_time_t app_debug_get_time_ms(void)
{
    return (debug_time_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
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
