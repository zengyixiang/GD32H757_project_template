#ifndef LVGL_H
#define LVGL_H

#include <stdint.h>

typedef struct {
    const char *name;
} lv_obj_t;

void lv_init(void);
void lv_tick_inc(uint32_t tick_ms);
void lv_timer_handler(void);
lv_obj_t *lv_scr_act(void);
void lv_scr_load(lv_obj_t *screen);
lv_obj_t *lv_label_create(lv_obj_t *parent);
void lv_label_set_text(lv_obj_t *label, const char *text);

#endif
