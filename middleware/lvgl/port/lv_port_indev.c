#include "lv_port_indev.h"

static lvgl_port_config_t lv_port_indev_config;

void lv_port_indev_init(const lvgl_port_config_t *config)
{
    if(config != 0) {
        lv_port_indev_config = *config;
    }

    if(lv_port_indev_config.touch_init != 0) {
        lv_port_indev_config.touch_init();
    }
}
