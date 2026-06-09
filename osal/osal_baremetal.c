#include "osal_baremetal.h"

#include "gd32h7xx.h"

static volatile uint32_t osal_tick_ms;
static volatile uint32_t osal_delay_counter;

void osal_baremetal_init(void)
{
    osal_tick_ms = 0U;
    osal_delay_counter = 0U;

    if(SysTick_Config(SystemCoreClock / 1000U)) {
        while(1) {
        }
    }

    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void osal_baremetal_tick_isr(void)
{
    osal_tick_ms++;

    if(osal_delay_counter != 0U) {
        osal_delay_counter--;
    }
}

uint32_t osal_baremetal_millis(void)
{
    return osal_tick_ms;
}

void osal_baremetal_delay_ms(uint32_t delay_ms)
{
    osal_delay_counter = delay_ms;

    while(osal_delay_counter != 0U) {
    }
}

void osal_baremetal_task_delay_ms(uint32_t delay_ms)
{
    osal_baremetal_delay_ms(delay_ms);
}

uint32_t osal_baremetal_enter_critical(void)
{
    uint32_t state = __get_PRIMASK();

    __disable_irq();

    return state;
}

void osal_baremetal_exit_critical(uint32_t state)
{
    __set_PRIMASK(state);
}

int osal_baremetal_task_create(const osal_task_config_t *config)
{
    (void)config;

    return -1;
}

void osal_baremetal_kernel_start(void)
{
}
