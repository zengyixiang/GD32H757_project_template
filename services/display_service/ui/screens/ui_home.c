#include "ui_home.h"

#include "app_event.h"
#include "lvgl.h"

void ui_home_show(void)
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *title = lv_label_create(screen);

    lv_label_set_text(title, "Home");
    lv_scr_load(screen);
}

void ui_home_on_send_button_clicked(void)
{
    const app_event_t event = {
        .id = APP_EVENT_UI_SEND_CMD,
    };

    (void)app_event_publish(&event);
}
