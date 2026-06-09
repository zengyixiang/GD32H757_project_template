#include "display_service.h"

#include "board_display.h"
#include "board_touch.h"
#include "lvgl_port.h"
#include "ui.h"

void display_service_init(void)
{
    const lvgl_port_config_t lvgl_config = {
        .display_init = board_display_init,
        .display_flush = board_display_flush,
        .touch_init = board_touch_init,
        .touch_read = board_touch_read,
    };

    lvgl_port_init(&lvgl_config);
    ui_init();
}

void display_service_poll(void)
{
    lvgl_port_tick();
    ui_tick();
    lvgl_port_timer_handler();
}

void display_service_switch_screen(display_screen_t screen)
{
    switch(screen) {
    case DISPLAY_SCREEN_HOME:
        ui_show_screen(UI_SCREEN_HOME);
        break;
    case DISPLAY_SCREEN_SETTINGS:
        ui_show_screen(UI_SCREEN_SETTINGS);
        break;
    default:
        ui_show_screen(UI_SCREEN_HOME);
        break;
    }
}
