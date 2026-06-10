file(GLOB BOARD_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

add_library(board_gd32h757_eval STATIC)
target_sources(board_gd32h757_eval PRIVATE
    ${BOARD_SOURCES}
)
target_include_directories(board_gd32h757_eval PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(board_gd32h757_eval PUBLIC
    bsp_lib
    components_lib
    middleware_lib
)
