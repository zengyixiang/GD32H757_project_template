#ifndef APP_EVENT_H
#define APP_EVENT_H

typedef enum {
    APP_EVENT_NONE = 0,
    APP_EVENT_SENSOR_READY,
    APP_EVENT_COMM_RX,
    APP_EVENT_UPGRADE_REQUEST,
} app_event_id_t;

void app_event_init(void);
void app_event_publish(app_event_id_t event);

#endif
