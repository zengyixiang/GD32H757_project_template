#ifndef BOARD_TOUCH_H
#define BOARD_TOUCH_H

#include <stdint.h>

void board_touch_init(void);
int board_touch_read(int16_t *x, int16_t *y, uint8_t *pressed);

#endif
