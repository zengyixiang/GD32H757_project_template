#include "osal_freertos.h"

#include "FreeRTOS.h"
#include "task.h"
#include "gd32h7xx.h"

static volatile uint32_t osal_freertos_tick_ms;

static int osal_freertos_scheduler_started(void)
{
    return xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
}

void osal_freertos_init(void)
{
    osal_freertos_tick_ms = 0U;

    if(SysTick_Config(SystemCoreClock / 1000U)) {
        while(1) {
        }
    }

    NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
}

void osal_freertos_tick_isr(void)
{
    osal_freertos_tick_ms++;
}

uint32_t osal_freertos_millis(void)
{
    if(osal_freertos_scheduler_started()) {
        return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    }

    return osal_freertos_tick_ms;
}

void osal_freertos_delay_ms(uint32_t delay_ms)
{
    osal_freertos_task_delay_ms(delay_ms);
}

void osal_freertos_task_delay_ms(uint32_t delay_ms)
{
    if(osal_freertos_scheduler_started()) {
        TickType_t ticks = pdMS_TO_TICKS(delay_ms);

        if((ticks == 0U) && (delay_ms != 0U)) {
            ticks = 1U;
        }

        vTaskDelay(ticks);
    } else {
        uint32_t start = osal_freertos_tick_ms;

        while((uint32_t)(osal_freertos_tick_ms - start) < delay_ms) {
        }
    }
}

uint32_t osal_freertos_enter_critical(void)
{
    if(osal_freertos_scheduler_started()) {
        taskENTER_CRITICAL();
        return 0U;
    }

    uint32_t state = __get_PRIMASK();

    __disable_irq();

    return state;
}

void osal_freertos_exit_critical(uint32_t state)
{
    if(osal_freertos_scheduler_started()) {
        (void)state;
        taskEXIT_CRITICAL();
    } else {
        __set_PRIMASK(state);
    }
}

int osal_freertos_task_create(const osal_task_config_t *config)
{
    if((config == 0) || (config->entry == 0)) {
        return -1;
    }

    if(xTaskCreate(config->entry,
                   config->name,
                   (configSTACK_DEPTH_TYPE)config->stack_size,
                   config->argument,
                   (UBaseType_t)config->priority,
                   NULL) != pdPASS) {
        return -1;
    }

    return 0;
}

void osal_freertos_kernel_start(void)
{
    vTaskStartScheduler();
}
