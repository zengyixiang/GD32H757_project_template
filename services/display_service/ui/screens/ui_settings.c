#include "ui_settings.h"

#include "lvgl.h"

void ui_settings_show(void)
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *title = lv_label_create(screen);

    lv_label_set_text(title, "Settings");
    lv_scr_load(screen);
}
