#ifndef OSAL_FREERTOS_H
#define OSAL_FREERTOS_H

#include "osal.h"

#include <stdint.h>

void osal_freertos_init(void);
void osal_freertos_tick_isr(void);
uint32_t osal_freertos_millis(void);
void osal_freertos_delay_ms(uint32_t delay_ms);
void osal_freertos_task_delay_ms(uint32_t delay_ms);
uint32_t osal_freertos_enter_critical(void);
void osal_freertos_exit_critical(uint32_t state);
int osal_freertos_task_create(const osal_task_config_t *config);
void osal_freertos_kernel_start(void);

#endif
