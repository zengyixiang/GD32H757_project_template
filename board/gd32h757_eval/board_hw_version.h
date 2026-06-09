#ifndef BOARD_HW_VERSION_H
#define BOARD_HW_VERSION_H

#include <stdint.h>

#define BOARD_HW_VERSION_UNKNOWN 0U
#define BOARD_HW_VERSION_MIN 1U
#define BOARD_HW_VERSION_MAX 30U

typedef uint8_t board_hw_version_t;

board_hw_version_t board_hw_version_get(void);
uint8_t board_hw_version_is_valid(void);

#endif
