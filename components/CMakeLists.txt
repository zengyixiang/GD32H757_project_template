add_library(components_lib STATIC)
target_sources(components_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ringbuffer/ringbuffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/protocol.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell/shell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cli/cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/storage/storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware_version/hardware_version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/eeprom/at24c02/at24c02.c
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/sensors/gxht30/gxht30.c
)
target_include_directories(components_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ringbuffer
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/shell
    ${CMAKE_CURRENT_SOURCE_DIR}/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/cli
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware_version
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/eeprom/at24c02
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/sensors/gxht30
)
target_compile_definitions(components_lib PUBLIC
    DEBUG_PRINTF=1
    DEBUG_U32_USE_UNSIGNED_LONG=1
    DEBUG_USE_COLOR=1
    DEBUG_PRINT_VERSION=1
    DEBUG_ENABLE_FLOAT=1
    DEBUG_FLUSH_MAX_PER_CALL=0
)
