#define LOG_TAG "app.debug"
#define LOG_LVL ELOG_LVL_INFO

#include "app_debug.h"

#include "board_uart.h"
#include "common.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "log.h"
#include "task.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define APP_DEBUG_DIAGNOSTICS_ENABLE 1

static void app_debug_write(const char *data, size_t size)
{
    board_uart_write_buffer(data, (int)size);
}

#if APP_DEBUG_DIAGNOSTICS_ENABLE
static CLI_Definition_List_Item_t app_debug_tasks_command_item;
static uint8_t app_debug_tasks_command_registered;

static BaseType_t app_debug_tasks_command(char *pcWriteBuffer,
                                          size_t xWriteBufferLen,
                                          const char *pcCommandString)
{
    int written;

    unused(pcCommandString);

    written = snprintf(pcWriteBuffer,
                       xWriteBufferLen,
                       "RTOS heap free=%u min=%u\r\n"
                       "Task list:\r\n",
                       (unsigned int)xPortGetFreeHeapSize(),
                       (unsigned int)xPortGetMinimumEverFreeHeapSize());

    if((written < 0) || ((size_t)written >= xWriteBufferLen)) {
        pcWriteBuffer[0] = '\0';
        return pdFALSE;
    }

    vTaskList(pcWriteBuffer + written);
    return pdFALSE;
}

static const CLI_Command_Definition_t app_debug_tasks_command_definition = {
    "tasks",
    "\r\ntasks:\r\n Lists FreeRTOS heap and task state information\r\n\r\n",
    app_debug_tasks_command,
    0
};

static void app_debug_register_cli_commands(void)
{
    if(app_debug_tasks_command_registered != 0U) {
        return;
    }

    if(FreeRTOS_CLIRegisterCommandStatic(&app_debug_tasks_command_definition,
                                         &app_debug_tasks_command_item) == pdPASS) {
        app_debug_tasks_command_registered = 1U;
    }
}
#endif

void app_debug_init(void)
{
    log_set_output(app_debug_write);
    unused(log_init());
#if APP_DEBUG_DIAGNOSTICS_ENABLE
    app_debug_register_cli_commands();
#endif
}

int app_debug_start(void)
{
    return 0;
}
