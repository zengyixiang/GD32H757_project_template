#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

typedef enum {
    DISPLAY_SCREEN_HOME = 0,
    DISPLAY_SCREEN_SETTINGS,
} display_screen_t;

void display_service_init(void);
int display_service_start(void);
int display_service_request_screen(display_screen_t screen);
int display_service_request_text(const char *text);

#endif
