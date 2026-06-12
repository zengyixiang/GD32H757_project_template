#define LOG_TAG "log"
#define LOG_LVL ELOG_LVL_INFO

#include "log.h"

#include <stdbool.h>

extern int log_port_start_async_task(void);

static bool log_started;

static void log_set_default_format(void)
{
    const size_t common_fmt = ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_T_INFO;
    const size_t debug_fmt = common_fmt | ELOG_FMT_DIR | ELOG_FMT_LINE;

    elog_set_fmt(ELOG_LVL_ASSERT, debug_fmt);
    elog_set_fmt(ELOG_LVL_ERROR, debug_fmt);
    elog_set_fmt(ELOG_LVL_WARN, debug_fmt);
    elog_set_fmt(ELOG_LVL_INFO, common_fmt);
    elog_set_fmt(ELOG_LVL_DEBUG, debug_fmt);
    elog_set_fmt(ELOG_LVL_VERBOSE, debug_fmt);
}

int log_init(void)
{
    if(log_started) {
        return 0;
    }

    if(elog_init() != ELOG_NO_ERR) {
        return -1;
    }

    log_set_default_format();

    if(log_port_start_async_task() != 0) {
        elog_set_output_enabled(true);
        elog_async_enabled(false);
        log_e("EasyLogger async output task create failed, fallback to sync output.");
        log_started = true;
        return -1;
    }

    elog_start();
    log_started = true;

    return 0;
}

