#include "board_touch.h"

void board_touch_init(void)
{
    /* my_product_v1 touch controller init placeholder. */
}

int board_touch_read(int16_t *x, int16_t *y, uint8_t *pressed)
{
    if(x != 0) {
        *x = 0;
    }
    if(y != 0) {
        *y = 0;
    }
    if(pressed != 0) {
        *pressed = 0U;
    }

    return 0;
}
