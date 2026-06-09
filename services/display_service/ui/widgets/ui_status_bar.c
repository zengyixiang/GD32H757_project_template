#include "ui_status_bar.h"

#include "lvgl.h"

static lv_obj_t *ui_status_label;

void ui_status_bar_init(void)
{
    ui_status_label = lv_label_create(lv_scr_act());
    lv_label_set_text(ui_status_label, "Ready");
}

void ui_status_bar_update(void)
{
    lv_label_set_text(ui_status_label, "Running");
}
