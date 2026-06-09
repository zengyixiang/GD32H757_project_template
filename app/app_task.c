#define DEBUG_LEVEL LVL_DEBUG

#include "app_task.h"

#include "comm_service.h"
#include "display_service.h"
#include "log.h"
#include "osal.h"
#include "project_config.h"
#include "sensor_service.h"
#include "upgrade_service.h"

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
    log_flush();
}

static void app_display_process(void)
{
    display_service_poll();
}

#if PROJECT_USE_FREERTOS
static void app_main_task(void *argument)
{
    (void)argument;

    while(1) {
        app_main_process();
        osal_task_delay_ms(1000U);
    }
}

static void app_display_task(void *argument)
{
    (void)argument;

    while(1) {
        app_display_process();
        osal_task_delay_ms(5U);
    }
}
#endif

void app_task_create(void)
{
#if PROJECT_USE_FREERTOS
    const osal_task_config_t main_task = {
        .name = "app_main",
        .entry = app_main_task,
        .argument = 0,
        .stack_size = 1024U,
        .priority = 2U,
    };
    const osal_task_config_t display_task = {
        .name = "display",
        .entry = app_display_task,
        .argument = 0,
        .stack_size = 1024U,
        .priority = 3U,
    };

    (void)osal_task_create(&main_task);
    (void)osal_task_create(&display_task);
#endif
}

void app_task_run(void)
{
    static uint32_t last_display_ms;
    static uint32_t last_main_ms;
    static uint8_t initialized;
    uint32_t now = osal_millis();

    if(initialized == 0U) {
        last_display_ms = now - 5U;
        last_main_ms = now - 1000U;
        initialized = 1U;
    }

    if((uint32_t)(now - last_display_ms) >= 5U) {
        last_display_ms = now;
        app_display_process();
    }

    if((uint32_t)(now - last_main_ms) >= 1000U) {
        last_main_ms = now;
        app_main_process();
    }
}
