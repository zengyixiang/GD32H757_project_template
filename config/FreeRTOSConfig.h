#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stddef.h>
#include <stdint.h>

extern uint32_t SystemCoreClock;

#define configCPU_CLOCK_HZ                         ((unsigned long)SystemCoreClock)
#define configTICK_RATE_HZ                         ((TickType_t)1000)
#define configTICK_TYPE_WIDTH_IN_BITS              TICK_TYPE_WIDTH_32_BITS

#define configUSE_PREEMPTION                       1
#define configUSE_TIME_SLICING                     1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#define configUSE_TICKLESS_IDLE                    0
#define configIDLE_SHOULD_YIELD                    1

#define configMAX_PRIORITIES                       8
#define configMINIMAL_STACK_SIZE                   256
#define configMAX_TASK_NAME_LEN                    16
#define configTOTAL_HEAP_SIZE                      ((size_t)(64U * 1024U))

#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0
#define configUSE_MALLOC_FAILED_HOOK               0
#define configCHECK_FOR_STACK_OVERFLOW             0

#define configSUPPORT_DYNAMIC_ALLOCATION           1
#define configSUPPORT_STATIC_ALLOCATION            0

#define configUSE_MUTEXES                          1
#define configUSE_RECURSIVE_MUTEXES                1
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_QUEUE_SETS                       0
#define configUSE_TIMERS                           0
#define configUSE_EVENT_GROUPS                     1
#define configUSE_STREAM_BUFFERS                   1
#define configUSE_TASK_NOTIFICATIONS               1

#define configPRIO_BITS                            4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY    15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY            (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY       (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

#define configCHECK_HANDLER_INSTALLATION           0

#define INCLUDE_vTaskDelay                         1
#define INCLUDE_xTaskDelayUntil                    1
#define INCLUDE_vTaskDelete                        1
#define INCLUDE_xTaskGetSchedulerState             1
#define INCLUDE_xTaskGetCurrentTaskHandle          1
#define INCLUDE_uxTaskPriorityGet                  1

#define configASSERT(x)                            \
    do {                                           \
        if((x) == 0) {                             \
            __asm volatile ("cpsid i" ::: "memory"); \
            for(;;) {                              \
            }                                      \
        }                                          \
    } while(0)

#endif
