#include "lvgl.h"

static lv_obj_t lvgl_default_screen = {
    .name = "default",
};
static lv_obj_t lvgl_label = {
    .name = "label",
};
static lv_obj_t *lvgl_active_screen = &lvgl_default_screen;
static uint32_t lvgl_tick_ms;

void lv_init(void)
{
    lvgl_tick_ms = 0U;
    lvgl_active_screen = &lvgl_default_screen;
}

void lv_tick_inc(uint32_t tick_ms)
{
    lvgl_tick_ms += tick_ms;
}

void lv_timer_handler(void)
{
    (void)lvgl_tick_ms;
}

lv_obj_t *lv_scr_act(void)
{
    return lvgl_active_screen;
}

void lv_scr_load(lv_obj_t *screen)
{
    if(screen != 0) {
        lvgl_active_screen = screen;
    }
}

lv_obj_t *lv_label_create(lv_obj_t *parent)
{
    (void)parent;

    return &lvgl_label;
}

void lv_label_set_text(lv_obj_t *label, const char *text)
{
    (void)label;
    (void)text;
}
