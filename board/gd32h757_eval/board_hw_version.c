#include "board_hw_version.h"

void board_hw_version_init(void)
{
}

board_hw_version_t board_hw_version_get(void)
{
    return BOARD_HW_VERSION_UNKNOWN;
}

uint8_t board_hw_version_is_valid(void)
{
    return 0U;
}
