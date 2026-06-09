#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include <stdint.h>

typedef void (*lvgl_port_void_fn_t)(void);
typedef void (*lvgl_port_flush_fn_t)(const void *area, const void *color_buffer);
typedef int (*lvgl_port_touch_read_fn_t)(int16_t *x, int16_t *y, uint8_t *pressed);

typedef struct {
    lvgl_port_void_fn_t display_init;
    lvgl_port_flush_fn_t display_flush;
    lvgl_port_void_fn_t touch_init;
    lvgl_port_touch_read_fn_t touch_read;
} lvgl_port_config_t;

void lvgl_port_init(const lvgl_port_config_t *config);
void lvgl_port_tick(void);
void lvgl_port_timer_handler(void);

#endif
