#define LOG_TAG "app.debug"
#define LOG_LVL ELOG_LVL_INFO

#include "app_debug.h"

#include "board_uart.h"
#include "common.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "log.h"
#include "task.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define APP_DEBUG_DIAGNOSTICS_ENABLE 1
#define APP_DEBUG_TASKS_MAX          24U
#define APP_DEBUG_TASKS_SEPARATOR    "+-----------------+-------+------+--------------+-----+\r\n"

static void app_debug_write(const char *data, size_t size)
{
    board_uart_write_buffer(data, (int)size);
}

#if APP_DEBUG_DIAGNOSTICS_ENABLE
static CLI_Definition_List_Item_t app_debug_tasks_command_item;
static uint8_t app_debug_tasks_command_registered;
static TaskStatus_t app_debug_task_status[APP_DEBUG_TASKS_MAX];

static char app_debug_task_state_to_char(eTaskState state)
{
    char state_char;

    switch(state) {
    case eRunning:
        state_char = 'X';
        break;
    case eReady:
        state_char = 'R';
        break;
    case eBlocked:
        state_char = 'B';
        break;
    case eSuspended:
        state_char = 'S';
        break;
    case eDeleted:
        state_char = 'D';
        break;
    case eInvalid:
    default:
        state_char = '?';
        break;
    }

    return state_char;
}

static BaseType_t app_debug_append(char *buffer, size_t buffer_len, size_t *used, const char *format, ...)
{
    va_list args;
    int written;

    if((buffer == NULL) || (used == NULL) || (format == NULL) || (*used >= buffer_len)) {
        return pdFALSE;
    }

    va_start(args, format);
    written = vsnprintf(buffer + *used, buffer_len - *used, format, args);
    va_end(args);

    if((written < 0) || ((size_t)written >= (buffer_len - *used))) {
        if(buffer_len != 0U) {
            buffer[buffer_len - 1U] = '\0';
        }
        return pdFALSE;
    }

    *used += (size_t)written;
    return pdTRUE;
}

static BaseType_t app_debug_tasks_command(char *pcWriteBuffer,
                                          size_t xWriteBufferLen,
                                          const char *pcCommandString)
{
    UBaseType_t task_count;
    UBaseType_t task_list_count;
    UBaseType_t i;
    size_t used = 0U;

    unused(pcCommandString);

    if(app_debug_append(pcWriteBuffer,
                        xWriteBufferLen,
                        &used,
                        "RTOS heap:\r\n"
                        "  free: %u bytes\r\n"
                        "  min : %u bytes\r\n\r\n",
                        (unsigned int)xPortGetFreeHeapSize(),
                        (unsigned int)xPortGetMinimumEverFreeHeapSize()) == pdFALSE) {
        return pdFALSE;
    }

    task_count = uxTaskGetNumberOfTasks();
    if(app_debug_append(pcWriteBuffer,
                        xWriteBufferLen,
                        &used,
                        "Tasks (%u):\r\n"
                        APP_DEBUG_TASKS_SEPARATOR
                        "| Name            | State | Prio | Stack(words) | Num |\r\n"
                        APP_DEBUG_TASKS_SEPARATOR,
                        (unsigned int)task_count) == pdFALSE) {
        return pdFALSE;
    }

    if(task_count > APP_DEBUG_TASKS_MAX) {
        if(app_debug_append(pcWriteBuffer,
                            xWriteBufferLen,
                            &used,
                            "| Too many tasks to display: %u > %u%16s|\r\n",
                            (unsigned int)task_count,
                            (unsigned int)APP_DEBUG_TASKS_MAX,
                            "") == pdFALSE) {
            return pdFALSE;
        }
    } else {
        task_list_count = uxTaskGetSystemState(app_debug_task_status, APP_DEBUG_TASKS_MAX, NULL);
        for(i = 0U; i < task_list_count; i++) {
            if(app_debug_append(pcWriteBuffer,
                                xWriteBufferLen,
                                &used,
                                "| %-15.15s |   %c   | %4u | %12u | %3u |\r\n",
                                app_debug_task_status[i].pcTaskName,
                                app_debug_task_state_to_char(app_debug_task_status[i].eCurrentState),
                                (unsigned int)app_debug_task_status[i].uxCurrentPriority,
                                (unsigned int)app_debug_task_status[i].usStackHighWaterMark,
                                (unsigned int)app_debug_task_status[i].xTaskNumber) == pdFALSE) {
                return pdFALSE;
            }
        }
    }

    (void)app_debug_append(pcWriteBuffer,
                           xWriteBufferLen,
                           &used,
                           APP_DEBUG_TASKS_SEPARATOR
                           "State: X=running, R=ready, B=blocked, S=suspended, D=deleted\r\n");
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
