#include "key_service.h"

#include "app_event.h"
#include "board_key.h"
#include "FreeRTOS.h"
#include "task.h"

#define KEY_TASK_STACK_WORDS 512U
#define KEY_TASK_PRIORITY 2U
#define KEY_TASK_PERIOD_MS 10U

static int key_service_last_user_state;

void key_service_init(void)
{
    key_service_last_user_state = 0;
}

static void key_service_process(void)
{
    int user_state = board_key_read(BOARD_KEY_USER);

    if((user_state != 0) && (key_service_last_user_state == 0)) {
        const app_event_t event = {
            .id = APP_EVENT_KEY_USER_PRESSED,
        };

        (void)app_event_publish(&event);
    }

    key_service_last_user_state = user_state;
}

static void key_service_task(void *argument)
{
    (void)argument;

    while(1) {
        key_service_process();
        vTaskDelay(pdMS_TO_TICKS(KEY_TASK_PERIOD_MS));
    }
}

int key_service_start(void)
{
    return xTaskCreate(key_service_task,
                       "key_task",
                       KEY_TASK_STACK_WORDS,
                       NULL,
                       KEY_TASK_PRIORITY,
                       NULL) == pdPASS ? 0 : -1;
}
