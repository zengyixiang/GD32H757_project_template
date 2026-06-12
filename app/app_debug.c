#define LOG_TAG "app.debug"
#define LOG_LVL ELOG_LVL_INFO

#include "app_debug.h"

#include "board_uart.h"
#include "common.h"
#include "FreeRTOS.h"
#include "log.h"
#include "task.h"
#include <string.h>
#include <stddef.h>

#define APP_DEBUG_DIAGNOSTICS_ENABLE 1

#define APP_DEBUG_TASK_STACK_WORDS 1024U
#define APP_DEBUG_TASK_PRIORITY    1U
#define APP_DEBUG_PERIOD_MS        5000U

static void app_debug_write(const char *data, size_t size)
{
    board_uart_write_buffer(data, (int)size);
}

#if APP_DEBUG_DIAGNOSTICS_ENABLE
static char app_debug_task_list[configSTATS_BUFFER_MAX_LENGTH];

static void app_debug_task(void *argument)
{
    unused(argument);

    while(1) {
        memset(app_debug_task_list, 0, sizeof(app_debug_task_list));
        vTaskList(app_debug_task_list);
        log_i("RTOS heap free=%u min=%u",
              (unsigned int)xPortGetFreeHeapSize(),
              (unsigned int)xPortGetMinimumEverFreeHeapSize());
        log_i("Task list:\r\n%s", app_debug_task_list);
        vTaskDelay(pdMS_TO_TICKS(APP_DEBUG_PERIOD_MS));
    }
}
#endif

void app_debug_init(void)
{
    log_set_output(app_debug_write);
    unused(log_init());
}

int app_debug_start(void)
{
#if APP_DEBUG_DIAGNOSTICS_ENABLE
    return xTaskCreate(app_debug_task,
                       "debug_task",
                       APP_DEBUG_TASK_STACK_WORDS,
                       NULL,
                       APP_DEBUG_TASK_PRIORITY,
                       NULL) == pdPASS ? 0 : -1;
#else
    return 0;
#endif
}
