#include "log.h"

#include "debug.h"

void log_init(void)
{
}

void log_flush(void)
{
    debug_log_flush();
}
