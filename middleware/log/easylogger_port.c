#include "log.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#ifndef LOG_OUTPUT_TASK_STACK_WORDS
#define LOG_OUTPUT_TASK_STACK_WORDS 512U
#endif

#ifndef LOG_OUTPUT_TASK_PRIORITY
#define LOG_OUTPUT_TASK_PRIORITY (tskIDLE_PRIORITY + 1U)
#endif

static log_output_fn_t log_output;
static SemaphoreHandle_t log_output_lock;
static SemaphoreHandle_t log_flush_lock;
static TaskHandle_t log_output_task_handle;
static char log_async_line_buf[ELOG_LINE_BUF_SIZE];

void log_port_flush_async_output(void);
void elog_port_output(const char *log, size_t size);

static BaseType_t log_port_can_flush(void)
{
    return ((xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) &&
            (xPortIsInsideInterrupt() == pdFALSE)) ? pdTRUE : pdFALSE;
}

static void log_output_task(void *argument)
{
    (void)argument;

    while(1) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        log_port_flush_async_output();
    }
}

void log_set_output(log_output_fn_t output)
{
    log_output = output;
}

int log_port_start_async_task(void)
{
    if(log_output_task_handle != NULL) {
        return 0;
    }

    if(xTaskCreate(log_output_task,
                   "elog",
                   LOG_OUTPUT_TASK_STACK_WORDS,
                   NULL,
                   LOG_OUTPUT_TASK_PRIORITY,
                   &log_output_task_handle) != pdPASS) {
        log_output_task_handle = NULL;
        return -1;
    }

    return 0;
}

void log_port_flush_async_output(void)
{
    size_t size;
    BaseType_t locked = pdFALSE;

    if(log_port_can_flush() != pdTRUE) {
        return;
    }

    if(log_flush_lock != NULL) {
        locked = xSemaphoreTake(log_flush_lock, portMAX_DELAY);
        if(locked != pdTRUE) {
            return;
        }
    }

    do {
        size = elog_async_get_line_log(log_async_line_buf, sizeof(log_async_line_buf));
        if(size > 0U) {
            elog_port_output(log_async_line_buf, size);
        }
    } while(size > 0U);

    if(locked == pdTRUE) {
        (void)xSemaphoreGive(log_flush_lock);
    }
}

ElogErrCode elog_port_init(void)
{
    if(log_output_lock == NULL) {
        log_output_lock = xSemaphoreCreateMutex();
    }
    if(log_flush_lock == NULL) {
        log_flush_lock = xSemaphoreCreateMutex();
    }

    return ELOG_NO_ERR;
}

ElogErrCode elog_port_deinit(void)
{
    log_output_task_handle = NULL;

    if(log_output_lock != NULL) {
        vSemaphoreDelete(log_output_lock);
        log_output_lock = NULL;
    }
    if(log_flush_lock != NULL) {
        vSemaphoreDelete(log_flush_lock);
        log_flush_lock = NULL;
    }

    return ELOG_NO_ERR;
}

void elog_port_output(const char *log, size_t size)
{
    if((log_output != NULL) && (log != NULL) && (size > 0U)) {
        log_output(log, size);
    }
}

void elog_port_output_lock(void)
{
    if(log_output_lock != NULL) {
        (void)xSemaphoreTake(log_output_lock, portMAX_DELAY);
    }
}

void elog_port_output_unlock(void)
{
    if(log_output_lock != NULL) {
        (void)xSemaphoreGive(log_output_lock);
    }
}

const char *elog_port_get_time(void)
{
    static char time_buf[16];
    const unsigned long tick_ms = (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS);

    (void)snprintf(time_buf, sizeof(time_buf), "%lu", tick_ms);

    return time_buf;
}

const char *elog_port_get_p_info(void)
{
    return "";
}

const char *elog_port_get_t_info(void)
{
    const char *name = pcTaskGetName(NULL);

    return (name != NULL) ? name : "unknown";
}

void elog_async_output_notice(void)
{
    if(log_output_task_handle != NULL) {
        (void)xTaskNotifyGive(log_output_task_handle);
    }
}
