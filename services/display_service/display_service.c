#include "display_service.h"

#include "board_display.h"
#include "board_touch.h"
#include "FreeRTOS.h"
#include "lvgl_port.h"
#include "queue.h"
#include "task.h"
#include "ui.h"

#include <string.h>

#define DISPLAY_COMMAND_QUEUE_LENGTH 8U
#define DISPLAY_COMMAND_TEXT_MAX_LEN 64U
#define DISPLAY_TASK_STACK_WORDS 1024U
#define DISPLAY_TASK_PRIORITY 3U
#define DISPLAY_TASK_PERIOD_MS 5U

typedef enum {
    DISPLAY_COMMAND_NONE = 0,
    DISPLAY_COMMAND_SWITCH_SCREEN,
    DISPLAY_COMMAND_SET_TEXT,
} display_command_id_t;

typedef struct {
    display_command_id_t id;
    union {
        display_screen_t screen;
        char text[DISPLAY_COMMAND_TEXT_MAX_LEN];
    } data;
} display_command_t;

static QueueHandle_t display_command_queue;

static void display_service_switch_screen(display_screen_t screen)
{
    switch(screen) {
    case DISPLAY_SCREEN_HOME:
        ui_show_screen(UI_SCREEN_HOME);
        break;
    case DISPLAY_SCREEN_SETTINGS:
        ui_show_screen(UI_SCREEN_SETTINGS);
        break;
    default:
        ui_show_screen(UI_SCREEN_HOME);
        break;
    }
}

static void display_service_process_commands(void)
{
    display_command_t command;

    if(display_command_queue == 0) {
        return;
    }

    while(xQueueReceive(display_command_queue, &command, 0) == pdPASS) {
        switch(command.id) {
        case DISPLAY_COMMAND_SWITCH_SCREEN:
            display_service_switch_screen(command.data.screen);
            break;
        case DISPLAY_COMMAND_SET_TEXT:
            ui_set_message_text(command.data.text);
            break;
        default:
            break;
        }
    }
}

static void display_service_process(void)
{
    display_service_process_commands();
    ui_tick();
    lvgl_port_timer_handler();
}

static void display_service_task(void *argument)
{
    (void)argument;

    while(1) {
        display_service_process();
        vTaskDelay(pdMS_TO_TICKS(DISPLAY_TASK_PERIOD_MS));
    }
}

void display_service_init(void)
{
    const lvgl_port_config_t lvgl_config = {
        .display_init = board_display_init,
        .display_flush = board_display_flush,
        .touch_init = board_touch_init,
        .touch_read = board_touch_read,
    };

    display_command_queue = xQueueCreate(DISPLAY_COMMAND_QUEUE_LENGTH, sizeof(display_command_t));

    lvgl_port_init(&lvgl_config);
    ui_init();
}

int display_service_start(void)
{
    return xTaskCreate(display_service_task,
                       "display_task",
                       DISPLAY_TASK_STACK_WORDS,
                       NULL,
                       DISPLAY_TASK_PRIORITY,
                       NULL) == pdPASS ? 0 : -1;
}

int display_service_request_screen(display_screen_t screen)
{
    const display_command_t command = {
        .id = DISPLAY_COMMAND_SWITCH_SCREEN,
        .data.screen = screen,
    };

    if(display_command_queue == 0) {
        return -1;
    }

    return xQueueSend(display_command_queue, &command, 0) == pdPASS ? 0 : -1;
}

int display_service_request_text(const char *text)
{
    display_command_t command = {
        .id = DISPLAY_COMMAND_SET_TEXT,
    };

    if(display_command_queue == 0) {
        return -1;
    }

    if(text != 0) {
        (void)strncpy(command.data.text, text, sizeof(command.data.text) - 1U);
        command.data.text[sizeof(command.data.text) - 1U] = '\0';
    }

    return xQueueSend(display_command_queue, &command, 0) == pdPASS ? 0 : -1;
}
