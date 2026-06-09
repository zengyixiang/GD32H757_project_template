#include "app_init.h"
#include "app_task.h"
#include "board.h"
#include "osal.h"
#include "project_config.h"

int main(void)
{
    board_init();
    osal_init();
    app_init();
    app_task_create();

#if PROJECT_USE_FREERTOS
    osal_kernel_start();
#endif

#if PROJECT_USE_FREERTOS
    while(1) {
    }
#else
    while(1) {
        app_task_run();
    }
#endif
}
