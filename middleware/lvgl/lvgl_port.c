#include "lvgl_port.h"

#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl.h"

static lvgl_port_config_t lvgl_port_config;
static volatile int lvgl_port_initialized;

void lvgl_port_init(const lvgl_port_config_t *config)
{
    if(config != 0) {
        lvgl_port_config = *config;
    }

    lv_init();
    lv_port_disp_init(&lvgl_port_config);
    lv_port_indev_init(&lvgl_port_config);

    lvgl_port_initialized = 1;
}

void lvgl_port_tick(void)
{
    if(lvgl_port_initialized != 0) {
        lv_tick_inc(1U);
    }
}

void lvgl_port_timer_handler(void)
{
    lv_timer_handler();
}
