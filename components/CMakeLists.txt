add_library(components_lib STATIC)
target_sources(components_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/protocol.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell/shell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cli/cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/storage/storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware_version/hardware_version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/eeprom/at24c02/at24c02.c
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/sensors/gxht30/gxht30.c
)
target_include_directories(components_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/shell
    ${CMAKE_CURRENT_SOURCE_DIR}/cli
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware_version
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/eeprom/at24c02
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/sensors/gxht30
)
target_link_libraries(components_lib PUBLIC
    middleware_lib
)
