#include "comm_service.h"

#include "protocol.h"

void comm_service_init(void)
{
    protocol_init();
}

void comm_service_poll(void)
{
    protocol_poll();
}
