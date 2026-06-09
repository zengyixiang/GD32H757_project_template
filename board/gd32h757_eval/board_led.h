#ifndef BOARD_LED_H
#define BOARD_LED_H

#include <stdbool.h>

typedef enum {
    BOARD_LED_STATUS = 0,
    BOARD_LED_COUNT
} board_led_id_t;

void board_led_init(void);
void board_led_set(board_led_id_t id, bool on);
void board_led_toggle(board_led_id_t id);

#endif
