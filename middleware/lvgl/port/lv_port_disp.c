#include "lv_port_disp.h"

static lvgl_port_config_t lv_port_disp_config;

void lv_port_disp_init(const lvgl_port_config_t *config)
{
    if(config != 0) {
        lv_port_disp_config = *config;
    }

    if(lv_port_disp_config.display_init != 0) {
        lv_port_disp_config.display_init();
    }
}

void lv_port_disp_flush_ready(void)
{
}
