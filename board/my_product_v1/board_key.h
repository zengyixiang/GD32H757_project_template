#ifndef BOARD_KEY_H
#define BOARD_KEY_H

typedef enum {
    BOARD_KEY_USER = 0,
    BOARD_KEY_COUNT
} board_key_id_t;

void board_key_init(void);
int board_key_read(board_key_id_t id);

#endif
