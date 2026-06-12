#define LOG_TAG "app.task"
#define LOG_LVL ELOG_LVL_DEBUG

#include "log.h"
#include "app_task.h"

#include "app_event.h"
#include "comm_service.h"
#include "display_service.h"
#include "gd32h7xx_fwdgt.h"
#include "key_service.h"
#include "lvgl_port.h"
#include "app_debug.h"
#include "FreeRTOS.h"
#include "task.h"

#define APP_EVENT_TASK_STACK_WORDS 1024U
#define APP_EVENT_TASK_PRIORITY 2U

static void app_dispatch_event(const app_event_t *event)
{
    if(event == 0) {
        return;
    }

    switch(event->id) {
    case APP_EVENT_KEY_USER_PRESSED:
        (void)display_service_request_screen(DISPLAY_SCREEN_SETTINGS);
        break;
    case APP_EVENT_UI_SEND_CMD:
        (void)comm_service_request_send_demo();
        break;
    case APP_EVENT_COMM_SHOW_TEXT:
        (void)display_service_request_text(event->data.text);
        break;
    case APP_EVENT_SENSOR_READY:
    case APP_EVENT_COMM_RX:
    case APP_EVENT_UPGRADE_REQUEST:
    default:
        break;
    }
}

void vApplicationIdleHook(void)
{
    fwdgt_counter_reload();
}

void vApplicationTickHook(void)
{
    lvgl_port_tick();
}

static void app_event_task(void *argument)
{
    (void)argument;

    while(1) {
        app_event_t event;

        if(app_event_get(&event, APP_EVENT_WAIT_FOREVER) == 0) {
            app_dispatch_event(&event);
        }
    }
}

void app_task_create(void)
{
    (void)xTaskCreate(app_event_task,
                      "app_event_task",
                      APP_EVENT_TASK_STACK_WORDS,
                      NULL,
                      APP_EVENT_TASK_PRIORITY,
                      NULL);

    (void)display_service_start();
    (void)comm_service_start();
    (void)key_service_start();
    (void)app_debug_start();
}
