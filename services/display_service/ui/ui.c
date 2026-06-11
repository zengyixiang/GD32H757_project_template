#include "ui.h"

#include "ui_home.h"
#include "ui_settings.h"
#include "ui_status_bar.h"

static ui_screen_id_t ui_current_screen;

void ui_init(void)
{
    ui_status_bar_init();
    ui_show_screen(UI_SCREEN_HOME);
}

void ui_show_screen(ui_screen_id_t screen)
{
    ui_current_screen = screen;

    switch(screen) {
    case UI_SCREEN_HOME:
        ui_home_show();
        break;
    case UI_SCREEN_SETTINGS:
        ui_settings_show();
        break;
    default:
        ui_home_show();
        break;
    }
}

void ui_set_message_text(const char *text)
{
    (void)text;
    (void)ui_current_screen;
}

void ui_tick(void)
{
    ui_status_bar_update();
    (void)ui_current_screen;
}
