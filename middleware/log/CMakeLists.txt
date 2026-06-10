set(EASYLOGGER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/EasyLogger")

add_library(log_lib STATIC)
target_sources(log_lib PRIVATE
    ${EASYLOGGER_ROOT}/easylogger/src/elog.c
    ${EASYLOGGER_ROOT}/easylogger/src/elog_async.c
    ${EASYLOGGER_ROOT}/easylogger/src/elog_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/easylogger_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log.c
)

target_include_directories(log_lib BEFORE PUBLIC
    # 这里的顺序很关键：
    # EasyLogger 的 elog.h 使用 #include <elog_cfg.h>，尖括号包含会按
    # compiler include path 的顺序查找。config/ 必须放在 EasyLogger/inc 前面，
    # 所以用 BEFORE 明确插到 include path 最前面，优先命中本工程的
    # config/elog_cfg.h，而不是子仓库自带的 EasyLogger/easylogger/inc/elog_cfg.h。
    ${CMAKE_SOURCE_DIR}/config
)

target_include_directories(log_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${EASYLOGGER_ROOT}/easylogger/inc
)

target_link_libraries(log_lib PRIVATE
    freertos_kernel_stub
)
