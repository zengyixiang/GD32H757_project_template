#ifndef UI_H
#define UI_H

typedef enum {
    UI_SCREEN_HOME = 0,
    UI_SCREEN_SETTINGS,
} ui_screen_id_t;

void ui_init(void);
void ui_show_screen(ui_screen_id_t screen);
void ui_tick(void);

#endif
