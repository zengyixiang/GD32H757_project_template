#ifndef APP_EVENT_H
#define APP_EVENT_H

#include <stdint.h>

#define APP_EVENT_TEXT_MAX_LEN 64U
#define APP_EVENT_U8_DATA_MAX_LEN 64U
#define APP_EVENT_WAIT_FOREVER UINT32_MAX

typedef enum {
    APP_EVENT_NONE = 0,
    APP_EVENT_SENSOR_READY,
    APP_EVENT_COMM_RX,
    APP_EVENT_UPGRADE_REQUEST,
    APP_EVENT_KEY_USER_PRESSED,
    APP_EVENT_UI_SEND_CMD,
    APP_EVENT_COMM_SHOW_TEXT,
} app_event_id_t;

typedef struct {
    app_event_id_t id;
    union {
        char text[APP_EVENT_TEXT_MAX_LEN];
        uint8_t u8_data[APP_EVENT_U8_DATA_MAX_LEN];
    } data;
} app_event_t;

void app_event_init(void);
int app_event_publish(const app_event_t *event);
int app_event_get(app_event_t *event, uint32_t timeout_ms);
int app_event_publish_from_isr(const app_event_t *event);

#endif
