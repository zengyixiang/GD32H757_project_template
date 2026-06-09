#include "osal.h"

#include "project_config.h"

#if PROJECT_USE_FREERTOS
#include "osal_freertos.h"
#else
#include "osal_baremetal.h"
#endif

void osal_init(void)
{
#if PROJECT_USE_FREERTOS
    osal_freertos_init();
#else
    osal_baremetal_init();
#endif
}

void osal_tick_isr(void)
{
#if PROJECT_USE_FREERTOS
    osal_freertos_tick_isr();
#else
    osal_baremetal_tick_isr();
#endif
}

uint32_t osal_millis(void)
{
#if PROJECT_USE_FREERTOS
    return osal_freertos_millis();
#else
    return osal_baremetal_millis();
#endif
}

void osal_delay_ms(uint32_t delay_ms)
{
#if PROJECT_USE_FREERTOS
    osal_freertos_delay_ms(delay_ms);
#else
    osal_baremetal_delay_ms(delay_ms);
#endif
}

void osal_task_delay_ms(uint32_t delay_ms)
{
#if PROJECT_USE_FREERTOS
    osal_freertos_task_delay_ms(delay_ms);
#else
    osal_baremetal_task_delay_ms(delay_ms);
#endif
}

uint32_t osal_enter_critical(void)
{
#if PROJECT_USE_FREERTOS
    return osal_freertos_enter_critical();
#else
    return osal_baremetal_enter_critical();
#endif
}

void osal_exit_critical(uint32_t state)
{
#if PROJECT_USE_FREERTOS
    osal_freertos_exit_critical(state);
#else
    osal_baremetal_exit_critical(state);
#endif
}

int osal_task_create(const osal_task_config_t *config)
{
#if PROJECT_USE_FREERTOS
    return osal_freertos_task_create(config);
#else
    return osal_baremetal_task_create(config);
#endif
}

void osal_kernel_start(void)
{
#if PROJECT_USE_FREERTOS
    osal_freertos_kernel_start();
#else
    osal_baremetal_kernel_start();
#endif
}
