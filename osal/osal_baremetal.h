#ifndef OSAL_BAREMETAL_H
#define OSAL_BAREMETAL_H

#include <stdint.h>

#include "osal.h"

void osal_baremetal_init(void);
void osal_baremetal_tick_isr(void);
uint32_t osal_baremetal_millis(void);
void osal_baremetal_delay_ms(uint32_t delay_ms);
void osal_baremetal_task_delay_ms(uint32_t delay_ms);
uint32_t osal_baremetal_enter_critical(void);
void osal_baremetal_exit_critical(uint32_t state);
int osal_baremetal_task_create(const osal_task_config_t *config);
void osal_baremetal_kernel_start(void);

#endif
