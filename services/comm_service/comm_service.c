#include "comm_service.h"

#include "app_event.h"
#include "FreeRTOS.h"
#include "protocol.h"
#include "queue.h"
#include "task.h"

#include <string.h>

#define COMM_COMMAND_QUEUE_LENGTH 8U
#define COMM_TASK_STACK_WORDS 1024U
#define COMM_TASK_PRIORITY 2U
#define COMM_TASK_PERIOD_MS 10U

typedef enum {
    COMM_COMMAND_NONE = 0,
    COMM_COMMAND_SEND_DEMO,
} comm_command_id_t;

typedef struct {
    comm_command_id_t id;
} comm_command_t;

static QueueHandle_t comm_command_queue;

static int comm_service_send_demo_protocol(void)
{
    return 0;
}

static void comm_service_process_commands(void)
{
    comm_command_t command;

    if(comm_command_queue == 0) {
        return;
    }

    while(xQueueReceive(comm_command_queue, &command, 0) == pdPASS) {
        switch(command.id) {
        case COMM_COMMAND_SEND_DEMO:
            (void)comm_service_send_demo_protocol();
            break;
        default:
            break;
        }
    }
}

static void comm_service_process(void)
{
    comm_service_process_commands();
    protocol_poll();
    /* Template: call comm_service_on_protocol_show_text() after parsing
       a protocol frame that should update the UI. */
}

static void comm_service_task(void *argument)
{
    (void)argument;

    while(1) {
        comm_service_process();
        vTaskDelay(pdMS_TO_TICKS(COMM_TASK_PERIOD_MS));
    }
}

void comm_service_on_protocol_show_text(const char *text)
{
    app_event_t event = {
        .id = APP_EVENT_COMM_SHOW_TEXT,
    };

    if(text != 0) {
        (void)strncpy(event.data.text, text, sizeof(event.data.text) - 1U);
        event.data.text[sizeof(event.data.text) - 1U] = '\0';
    }

    (void)app_event_publish(&event);
}

void comm_service_init(void)
{
    comm_command_queue = xQueueCreate(COMM_COMMAND_QUEUE_LENGTH, sizeof(comm_command_t));
    protocol_init();
}

int comm_service_start(void)
{
    return xTaskCreate(comm_service_task,
                       "comm_task",
                       COMM_TASK_STACK_WORDS,
                       NULL,
                       COMM_TASK_PRIORITY,
                       NULL) == pdPASS ? 0 : -1;
}

int comm_service_request_send_demo(void)
{
    const comm_command_t command = {
        .id = COMM_COMMAND_SEND_DEMO,
    };

    if(comm_command_queue == 0) {
        return -1;
    }

    return xQueueSend(comm_command_queue, &command, 0) == pdPASS ? 0 : -1;
}
