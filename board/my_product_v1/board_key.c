#include "board_key.h"

#include "bsp_key.h"

void board_key_init(void)
{
    bsp_key_init();
}

int board_key_read(board_key_id_t id)
{
    if(id >= BOARD_KEY_COUNT) {
        return 0;
    }

    return bsp_key_read();
}
