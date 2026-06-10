#include "app_init.h"
#include "app_task.h"
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#define APP_START_TASK_STACK_WORDS 1024U
#define APP_START_TASK_PRIORITY    4U

static void app_start_task(void *argument)
{
    (void)argument;

    board_init();
    app_init();
    app_task_create();

    vTaskDelete(NULL);
}

int main(void)
{
    board_early_init();

    if(xTaskCreate(app_start_task,
                   "app_start",
                   APP_START_TASK_STACK_WORDS,
                   NULL,
                   APP_START_TASK_PRIORITY,
                   NULL) == pdPASS) {
        vTaskStartScheduler();
    }

    while(1) {
    }
}
