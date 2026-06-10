#ifndef ELOG_CFG_H
#define ELOG_CFG_H

/*
 * Project-owned EasyLogger configuration.
 *
 * EasyLogger also ships an easylogger/inc/elog_cfg.h, but that file belongs to
 * the third-party submodule. CMake puts this config/ directory before
 * EasyLogger's include directory, so <elog_cfg.h> resolves to this file first.
 */

#define ELOG_OUTPUT_ENABLE
#define ELOG_OUTPUT_LVL                          ELOG_LVL_VERBOSE
#define ELOG_ASSERT_ENABLE

#define ELOG_LINE_BUF_SIZE                       512
#define ELOG_LINE_NUM_MAX_LEN                    5
#define ELOG_FILTER_TAG_MAX_LEN                  30
#define ELOG_FILTER_KW_MAX_LEN                   16
#define ELOG_FILTER_TAG_LVL_MAX_NUM              5
#define ELOG_NEWLINE_SIGN                        "\r\n"

#define ELOG_COLOR_ENABLE
#define ELOG_COLOR_ASSERT                        (F_MAGENTA B_NULL S_NORMAL)
#define ELOG_COLOR_ERROR                         (F_RED B_NULL S_NORMAL)
#define ELOG_COLOR_WARN                          (F_YELLOW B_NULL S_NORMAL)
#define ELOG_COLOR_INFO                          (F_CYAN B_NULL S_NORMAL)
#define ELOG_COLOR_DEBUG                         (F_GREEN B_NULL S_NORMAL)
#define ELOG_COLOR_VERBOSE                       (F_BLUE B_NULL S_NORMAL)

#define ELOG_FMT_USING_DIR
#define ELOG_FMT_USING_LINE

#define ELOG_ASYNC_OUTPUT_ENABLE
#define ELOG_ASYNC_OUTPUT_LVL                    ELOG_LVL_ASSERT
#define ELOG_ASYNC_OUTPUT_BUF_SIZE               (ELOG_LINE_BUF_SIZE * 8)
#define ELOG_ASYNC_LINE_OUTPUT

/*
 * Do not define ELOG_ASYNC_OUTPUT_USING_PTHREAD here.
 * On this target the async consumer is a FreeRTOS task implemented by the
 * project port outside middleware/log/EasyLogger/.
 */

#endif /* ELOG_CFG_H */
