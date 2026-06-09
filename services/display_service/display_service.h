#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

typedef enum {
    DISPLAY_SCREEN_HOME = 0,
    DISPLAY_SCREEN_SETTINGS,
} display_screen_t;

void display_service_init(void);
void display_service_poll(void);
void display_service_switch_screen(display_screen_t screen);

#endif
