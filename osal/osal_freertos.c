#include "osal_freertos.h"

#include "gd32h7xx.h"

#ifndef OSAL_FREERTOS_REAL_KERNEL
#define OSAL_FREERTOS_REAL_KERNEL 0
#endif

static volatile uint32_t osal_freertos_tick_ms;

void osal_freertos_init(void)
{
    osal_freertos_tick_ms = 0U;

#if !OSAL_FREERTOS_REAL_KERNEL
    if(SysTick_Config(SystemCoreClock / 1000U)) {
        while(1) {
        }
    }

    NVIC_SetPriority(SysTick_IRQn, 0x00U);
#endif
}

void osal_freertos_tick_isr(void)
{
    osal_freertos_tick_ms++;
}

uint32_t osal_freertos_millis(void)
{
    return osal_freertos_tick_ms;
}

void osal_freertos_delay_ms(uint32_t delay_ms)
{
    osal_freertos_task_delay_ms(delay_ms);
}

void osal_freertos_task_delay_ms(uint32_t delay_ms)
{
    uint32_t start = osal_freertos_tick_ms;

    while((uint32_t)(osal_freertos_tick_ms - start) < delay_ms) {
    }
}

uint32_t osal_freertos_enter_critical(void)
{
    uint32_t state = __get_PRIMASK();

    __disable_irq();

    return state;
}

void osal_freertos_exit_critical(uint32_t state)
{
    __set_PRIMASK(state);
}

int osal_freertos_task_create(const osal_task_config_t *config)
{
    if((config == 0) || (config->entry == 0)) {
        return -1;
    }

    return 0;
}

void osal_freertos_kernel_start(void)
{
}
