file(GLOB_RECURSE SERVICES_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.c
)

add_library(services_lib STATIC)
target_sources(services_lib PRIVATE
    ${SERVICES_SOURCES}
)
target_include_directories(services_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/comm_service
    ${CMAKE_CURRENT_SOURCE_DIR}/key_service
    ${CMAKE_CURRENT_SOURCE_DIR}/sensor_service
    ${CMAKE_CURRENT_SOURCE_DIR}/display_service
    ${CMAKE_CURRENT_SOURCE_DIR}/upgrade_service
    ${CMAKE_CURRENT_SOURCE_DIR}/app_event
    ${CMAKE_CURRENT_SOURCE_DIR}/storage_service
    ${CMAKE_CURRENT_SOURCE_DIR}/display_service/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/display_service/ui/screens
    ${CMAKE_CURRENT_SOURCE_DIR}/display_service/ui/widgets
)
target_link_libraries(services_lib PUBLIC
    project_board
    components_lib
    middleware_lib
)
