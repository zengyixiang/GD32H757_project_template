target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/config
)

add_library(app_lib STATIC)
target_sources(app_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/app_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_debug.c
)
target_include_directories(app_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/config
)
target_link_libraries(app_lib PUBLIC
    services_lib
    project_board
    components_lib
    middleware_lib
    osal_lib
)
