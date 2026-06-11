#ifndef COMM_SERVICE_H
#define COMM_SERVICE_H

void comm_service_init(void);
int comm_service_start(void);
int comm_service_request_send_demo(void);
void comm_service_on_protocol_show_text(const char *text);

#endif
