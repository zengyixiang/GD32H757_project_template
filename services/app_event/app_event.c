#include "app_event.h"

#define LOG_TAG "app_event"
#define LOG_LVL ELOG_LVL_DEBUG

#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define APP_EVENT_QUEUE_LENGTH 16U

static QueueHandle_t app_event_queue;

void app_event_init(void)
{
    app_event_queue = xQueueCreate(APP_EVENT_QUEUE_LENGTH, sizeof(app_event_t));
}

int app_event_publish(const app_event_t *event)
{
    if((app_event_queue == 0) || (event == 0)) {
        return -1;
    }

    return xQueueSend(app_event_queue, event, 0) == pdPASS ? 0 : -1;
}

int app_event_get(app_event_t *event, uint32_t timeout_ms)
{
    TickType_t timeout_ticks = 0;

    if((app_event_queue == 0) || (event == 0)) {
        return -1;
    }

    timeout_ticks = timeout_ms == APP_EVENT_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);

    return xQueueReceive(app_event_queue, event, timeout_ticks) == pdPASS ? 0 : -1;
}

int app_event_publish_from_isr(const app_event_t *event)
{
    BaseType_t higher_priority_task_woken = pdFALSE;
    BaseType_t ret = pdFAIL;

    if((app_event_queue == 0) || (event == 0)) {
        return -1;
    }

    ret = xQueueSendFromISR(app_event_queue, event, &higher_priority_task_woken);

    portYIELD_FROM_ISR(higher_priority_task_woken);

    return ret == pdPASS ? 0 : -1;
}
