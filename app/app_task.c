#define DEBUG_LEVEL LVL_DEBUG

#include "app_task.h"

#include "comm_service.h"
#include "display_service.h"
#include "gd32h7xx_fwdgt.h"
#include "log.h"
#include "sensor_service.h"
#include "upgrade_service.h"

#include "FreeRTOS.h"
#include "task.h"

#define APP_MAIN_TASK_STACK_WORDS    1024U
#define APP_MAIN_TASK_PRIORITY       2U
#define APP_DISPLAY_TASK_STACK_WORDS 1024U
#define APP_DISPLAY_TASK_PRIORITY    3U

static void app_main_process(void)
{
    unsigned short sample = sensor_service_read_vbat();
    sensor_service_environment_t environment;

    LOG_D("VBAT sample: %u", sample);
    (void)sample;

    if(sensor_service_read_environment(&environment) == 0) {
        LOG_D("GXHT30: %ld mC, %lu m%%",
              (long)environment.temperature_mc,
              (unsigned long)environment.humidity_milli_percent);
    }

    comm_service_poll();
    upgrade_service_poll();
}

static void app_display_process(void)
{
    display_service_poll();
}

void vApplicationIdleHook(void)
{
    log_flush();
    fwdgt_counter_reload();
}

void vApplicationTickHook(void)
{
}

static void app_main_task(void *argument)
{
    (void)argument;

    while(1) {
        app_main_process();
        vTaskDelay(pdMS_TO_TICKS(1000U));
    }
}

static void app_display_task(void *argument)
{
    (void)argument;

    while(1) {
        app_display_process();
        vTaskDelay(pdMS_TO_TICKS(5U));
    }
}

void app_task_create(void)
{
    (void)xTaskCreate(app_main_task,
                      "app_main",
                      APP_MAIN_TASK_STACK_WORDS,
                      NULL,
                      APP_MAIN_TASK_PRIORITY,
                      NULL);

    (void)xTaskCreate(app_display_task,
                      "display",
                      APP_DISPLAY_TASK_STACK_WORDS,
                      NULL,
                      APP_DISPLAY_TASK_PRIORITY,
                      NULL);
}
