#ifndef FREERTOS_KERNEL_H
#define FREERTOS_KERNEL_H

#include <stdint.h>

void freertos_kernel_init(void);
void freertos_kernel_start(void);
void freertos_task_delay_ms(uint32_t delay_ms);

#endif
