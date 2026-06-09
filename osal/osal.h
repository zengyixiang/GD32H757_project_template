#ifndef OSAL_H
#define OSAL_H

#include <stdint.h>

typedef void (*osal_task_entry_t)(void *argument);

typedef struct {
    const char *name;
    osal_task_entry_t entry;
    void *argument;
    uint16_t stack_size;
    uint8_t priority;
} osal_task_config_t;

void osal_init(void);
void osal_tick_isr(void);
uint32_t osal_millis(void);
void osal_delay_ms(uint32_t delay_ms);
void osal_task_delay_ms(uint32_t delay_ms);
uint32_t osal_enter_critical(void);
void osal_exit_critical(uint32_t state);
int osal_task_create(const osal_task_config_t *config);
void osal_kernel_start(void);

#endif
