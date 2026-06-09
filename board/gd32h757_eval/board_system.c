#include "board_system.h"

#include "gd32h7xx.h"

void board_system_init(void)
{
    board_system_cache_enable();
    board_system_interrupt_priority_init();
}

void board_system_cache_enable(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
}

void board_system_interrupt_priority_init(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
}
